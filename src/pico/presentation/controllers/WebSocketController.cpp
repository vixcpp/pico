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
#include <pico/support/JsonHelpers.hpp>
#include <vix/json.hpp>

namespace pico::presentation::controllers
{
  void WebSocketController::register_routes(
      vix::App &app,
      application::services::RuntimeStatusService &runtime_status,
      application::services::EventService &events)
  {
    app.get("/api/ws/metrics",
            [&runtime_status, &events](vix::Request &, vix::Response &res)
            {
              using namespace vix::json;

              const auto messages = runtime_status.increment_ws_messages();

              const auto payload = o(
                  "route", "/api/ws/metrics",
                  "ws_messages", messages);

              (void)events.record(
                  "ws",
                  "ws.metrics.checked",
                  dumps_compact(payload));

              res.json(o(
                  "ok", true,
                  "websocket", o("enabled", true, "endpoint", "/ws", "messages", messages)));
            });

    app.get("/api/ws/info",
            [&events](vix::Request &, vix::Response &res)
            {
              using namespace vix::json;

              const auto payload = o(
                  "endpoint", "/ws",
                  "mode", "diagnostic");

              (void)events.record(
                  "ws",
                  "ws.info.checked",
                  dumps_compact(payload));

              res.json(o(
                  "ok", true,
                  "websocket", o("endpoint", "/ws", "raw_echo", true, "typed_ping_pong", true, "broadcast_room", "pico")));
            });
  }
}
