/**
 *
 *  @file HomeController.cpp
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

#include <pico/presentation/controllers/HomeController.hpp>

#include <vix.hpp>
#include <vix/json.hpp>

namespace pico::presentation::controllers
{
  void HomeController::register_routes(vix::App &app)
  {
    app.get("/api", [](vix::Request &req, vix::Response &res)
            {
      (void)req;

      using namespace vix::json;

      res.json(o(
          "ok", true,
          "service", "pico",
          "message", "Vix backend is running")); });
  }
}
