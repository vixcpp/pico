/**
 *
 *  @file StatusController.cpp
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

#include <pico/presentation/controllers/StatusController.hpp>
#include <pico/support/JsonHelpers.hpp>

namespace pico::presentation::controllers
{
  void StatusController::register_routes(
      vix::App &app,
      application::services::RuntimeStatusService &runtime_status)
  {
    app.get("/api/status",
            [&runtime_status](vix::Request &, vix::Response &res)
            {
              res.json(runtime_status.status());
            });

    app.get("/api/threadpool",
            [&runtime_status](vix::Request &, vix::Response &res)
            {
              res.json(runtime_status.threadpool_status());
            });
  }
}
