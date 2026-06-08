/**
 * @file AppBootstrap.cpp
 * @brief Startup implementation for the pico backend.
 */

#include <pico/app/AppBootstrap.hpp>
#include <pico/presentation/middleware/MiddlewareRegistry.hpp>
#include <pico/presentation/routes/RouteRegistry.hpp>

#include <vix.hpp>
#include <vix/log.hpp>
#include <vix/middleware/app/adapter.hpp>
#include <vix/middleware/performance/compression.hpp>
#include <vix/middleware/performance/static_compression.hpp>

namespace pico::app
{
  int AppBootstrap::run()
  {
    vix::config::Config cfg{".env"};
    vix::App app;

    const std::string viewsPath = cfg.getString("templates.path", "views");
    const std::string publicPath = cfg.getString("public.path", "public");
    const std::string publicMount = cfg.getString("public.mount", "/");
    const std::string publicIndex = cfg.getString("public.index", "index.html");
    const std::string publicCacheControl = cfg.getString("public.cache_control", "public, max-age=3600");
    const bool publicSpaFallback = cfg.getBool("public.spa_fallback", false);
    const bool publicCompression = cfg.getBool("public.compression", false);
    const int publicCompressionMinSize = cfg.getInt("public.compression_min_size", 1024);

    if (publicCompression)
    {
      const auto compressionOptions = vix::middleware::performance::CompressionOptions{
          .min_size = static_cast<std::size_t>(publicCompressionMinSize),
          .add_vary = true,
          .enabled = true,
      };

      auto compressionMiddleware = vix::middleware::app::adapt_ctx(
          vix::middleware::performance::compression(compressionOptions));

      app.use(std::move(compressionMiddleware));

     vix::App::set_static_response_hook(
          vix::middleware::performance::compressed_static_response_hook(compressionOptions));
    }

    app.templates(viewsPath);
    app.static_dir(
        publicPath,
        publicMount,
        publicIndex,
        true,
        publicCacheControl,
        true,
        publicSpaFallback);

    presentation::middleware::MiddlewareRegistry::register_all(app);
    presentation::routes::RouteRegistry::register_all(app);

    vix::log::info("Starting pico on port {}", cfg.getServerPort());

    app.run(cfg);
    return 0;
  }
} // namespace pico::app
