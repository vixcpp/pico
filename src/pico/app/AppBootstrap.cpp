/**
 *
 *  @file AppBootstrap.cpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2026, Gaspard Kirira.
 *  All rights reserved.
 *  https://github.com/vixcpp/pico
 *
 *  Use of this source code is governed by a MIT license
 *  that can be found in the License file.
 *
 *  Pico
 *
 */

#include <pico/app/AppBootstrap.hpp>

#include <pico/application/services/EventService.hpp>
#include <pico/application/services/JobService.hpp>
#include <pico/application/services/RuntimeStatusService.hpp>
#include <pico/infrastructure/database/DatabaseProvider.hpp>
#include <pico/infrastructure/database/EventRepository.hpp>
#include <pico/infrastructure/kv/PicoKvStore.hpp>
#include <pico/infrastructure/threadpool/PicoThreadPool.hpp>
#include <pico/presentation/middleware/MiddlewareRegistry.hpp>
#include <pico/presentation/routes/RouteRegistry.hpp>

#include <cstddef>
#include <exception>
#include <string>

#include <vix.hpp>
#include <vix/log.hpp>
#include <vix/middleware.hpp>
#include <vix/middleware/app/adapter.hpp>
#include <vix/middleware/performance/compression.hpp>
#include <vix/middleware/performance/static_compression.hpp>

namespace pico::app
{
  namespace
  {
    [[nodiscard]] std::string config_string(
        vix::config::Config &cfg,
        const std::string &key,
        const std::string &fallback)
    {
      return cfg.getString(key, fallback);
    }

    [[nodiscard]] bool config_bool(
        vix::config::Config &cfg,
        const std::string &key,
        bool fallback)
    {
      return cfg.getBool(key, fallback);
    }

    [[nodiscard]] int config_int(
        vix::config::Config &cfg,
        const std::string &key,
        int fallback)
    {
      return cfg.getInt(key, fallback);
    }
  }

  int AppBootstrap::run()
  {
    try
    {
      vix::config::Config cfg{".env"};
      vix::App app;

      const std::string views_path = config_string(cfg, "views.path", "views");
      const std::string public_path = config_string(cfg, "public.path", "public");
      const std::string public_mount = config_string(cfg, "public.mount", "/");
      const std::string public_index = config_string(cfg, "public.index", "index.html");
      const std::string public_cache_control = config_string(
          cfg,
          "public.cache_control",
          "public, max-age=3600");

      const bool public_spa_fallback = config_bool(
          cfg,
          "public.spa_fallback",
          false);

      const bool public_compression = config_bool(
          cfg,
          "public.compression",
          false);

      const int public_compression_min_size = config_int(
          cfg,
          "public.compression_min_size",
          1024);

      const std::string database_path = config_string(
          cfg,
          "database.sqlite_path",
          "storage/pico.db");

      infrastructure::database::DatabaseProvider database{database_path};
      database.open();

      infrastructure::database::EventRepository event_repository{database};

      infrastructure::kv::PicoKvStore kv{"storage/kv"};
      kv.open();

      infrastructure::threadpool::PicoThreadPool pool{4};

      application::services::EventService events{event_repository};
      events.initialize();

      application::services::RuntimeStatusService runtime_status{
          kv,
          pool,
          events};

      application::services::JobService jobs{
          pool,
          kv,
          events};

      const auto boot_count = runtime_status.mark_boot();

      vix::log::info("Pico boot count {}", boot_count);

      if (public_compression)
      {
        const auto compression_options =
            vix::middleware::performance::CompressionOptions{
                .min_size = static_cast<std::size_t>(public_compression_min_size),
                .add_vary = true,
                .enabled = true,
            };

        auto compression_middleware = vix::middleware::app::adapt_ctx(
            vix::middleware::performance::compression(compression_options));

        app.use(std::move(compression_middleware));

        vix::App::set_static_response_hook(
            vix::middleware::performance::compressed_static_response_hook(
                compression_options));
      }

      app.templates(views_path);

      app.static_dir(
          public_path,
          public_mount,
          public_index,
          true,
          public_cache_control,
          true,
          public_spa_fallback);

      presentation::middleware::MiddlewareRegistry::register_all(app);

      presentation::routes::RouteRegistry::register_all(
          app,
          runtime_status,
          kv,
          events,
          jobs);

      vix::log::info("Starting Pico on port {}", cfg.getServerPort());

      app.run(cfg);

      pool.wait_idle();
      pool.shutdown();
      kv.close();

      return 0;
    }
    catch (const std::exception &error)
    {
      vix::log::critical("Pico startup failed: {}", error.what());
      return 1;
    }
  }
}
