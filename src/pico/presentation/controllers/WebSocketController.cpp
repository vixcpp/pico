/**
 *
 *  @file WebSocketController.cpp
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

#include <pico/presentation/controllers/WebSocketController.hpp>
#include <vix/json.hpp>

namespace pico::presentation::controllers
{
  void WebSocketController::register_routes(
      vix::App &app,
      application::services::RuntimeStatusService &runtime_status,
      application::services::EventService &events)
  {
    (void)events;

    app.get("/api/ws/metrics",
            [&runtime_status](vix::Request &, vix::Response &res)
            {
              using namespace vix::json;

              res.json(o(
                  "ok", true,
                  "websocket", o("enabled", true, "endpoint", "/ws", "messages", runtime_status.ws_messages())));
            });

    app.get("/api/ws/info",
            [](vix::Request &, vix::Response &res)
            {
              using namespace vix::json;

              res.json(o(
                  "ok", true,
                  "websocket", o("endpoint", "/ws", "raw_echo", true, "typed_ping_pong", true, "broadcast_room", "pico")));
            });
  }
}
