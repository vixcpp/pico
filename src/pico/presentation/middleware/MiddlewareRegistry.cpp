/**
 *
 *  @file MiddlewareRegistry.cpp
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

#include <pico/presentation/middleware/MiddlewareRegistry.hpp>

#include <vix.hpp>
#include <vix/log.hpp>
#include <vix/middleware.hpp>
#include <vix/middleware/app/presets.hpp>

namespace pico::presentation::middleware
{
  void MiddlewareRegistry::register_all(vix::App &app)
  {
    /*
     * Active middleware order:
     *
     * CORS
     * rate limit
     * request logging
     * security headers
     * body limits
     * custom API headers
     * routes
     */

    app.use("/api", vix::middleware::app::cors_dev({"http://localhost:8080",
                                                    "http://127.0.0.1:8080"}));

    app.use("/api", vix::middleware::app::rate_limit_dev());

    app.use([](vix::Request &req, vix::Response &res, vix::App::Next next)
            {
              (void)res;

              vix::log::info("{} {}", req.method(), req.path());

              next(); });

    app.use(vix::middleware::app::security_headers_dev(false));

    app.use("/api", vix::middleware::app::body_limit_write_dev(1024 * 1024));

    app.use("/api", [](vix::Request &req, vix::Response &res, vix::App::Next next)
            {
              (void)req;

              res.header("X-API", "true");
              res.header("X-Pico", "true");

              next(); });
  }
}
