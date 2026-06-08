/**
 *
 *  @file HealthController.cpp
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

#include <pico/presentation/controllers/HealthController.hpp>

#include <vix.hpp>
#include <vix/json.hpp>

namespace pico::presentation::controllers
{
  void HealthController::register_routes(vix::App &app)
  {
    app.get("/health", [](vix::Request &req, vix::Response &res)
            {
      (void)req;

      using namespace vix::json;

      res.json(o(
          "ok", true,
          "status", "ok",
          "service", "pico")); });

    app.get("/api/health", [](vix::Request &req, vix::Response &res)
            {
      (void)req;

      using namespace vix::json;

      res.json(o(
          "ok", true,
          "status", "ok",
          "service", "pico",
          "api", true)); });
  }
}
