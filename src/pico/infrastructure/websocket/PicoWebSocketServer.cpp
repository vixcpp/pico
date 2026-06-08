/**
 *
 *  @file PicoWebSocketServer.cpp
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

#include <pico/infrastructure/websocket/PicoWebSocketServer.hpp>

#include <memory>
#include <string>

#include <vix/config/Config.hpp>
#include <vix/executor/RuntimeExecutor.hpp>
#include <vix/json.hpp>
#include <vix/log.hpp>
#include <vix/websocket/server.hpp>
#include <vix/websocket/session.hpp>

namespace pico::infrastructure::websocket
{
  PicoWebSocketServer::PicoWebSocketServer(
      vix::config::Config &cfg,
      application::services::RuntimeStatusService &runtime_status,
      application::services::EventService &events)
      : runtime_status_(&runtime_status),
        events_(&events),
        executor_(std::make_shared<vix::executor::RuntimeExecutor>(4)),
        server_(std::make_unique<vix::websocket::Server>(cfg, executor_))
  {
    configure_handlers();
  }

  PicoWebSocketServer::~PicoWebSocketServer() = default;

  vix::websocket::Server &PicoWebSocketServer::server()
  {
    return *server_;
  }

  const vix::websocket::Server &PicoWebSocketServer::server() const
  {
    return *server_;
  }

  std::shared_ptr<vix::executor::RuntimeExecutor>
  PicoWebSocketServer::executor() const
  {
    return executor_;
  }

  void PicoWebSocketServer::configure_handlers()
  {
    server_->on_open(
        [this](vix::websocket::Session &session)
        {
          (void)session;

          using namespace vix::json;

          const auto payload = o(
              "event", "open",
              "endpoint", "/ws");

          (void)events_->record(
              "ws",
              "ws.open",
              dumps_compact(payload));

          vix::log::info("Pico WebSocket client connected");
        });

    server_->on_message(
        [this](
            vix::websocket::Session &session,
            const std::string &message)
        {
          using namespace vix::json;

          const auto count = runtime_status_->increment_ws_messages();
          const auto parsed = try_loads(message);

          if (parsed.has_value())
          {
            const auto type = get_or<std::string>(*parsed, "type", "");

            if (type == "ping")
            {
              const auto response = o(
                  "type", "pong",
                  "payload", o("app", "pico", "runtime", "vix.cpp", "messages", count));

              session.send_text(dumps_compact(response));

              (void)events_->record(
                  "ws",
                  "ws.ping",
                  dumps_compact(response));

              return;
            }
          }

          const auto payload = o(
              "message", message,
              "messages", count);

          (void)events_->record(
              "ws",
              "ws.message",
              dumps_compact(payload));

          session.send_text("echo: " + message);
        });

    server_->on_typed_message(
        [this](
            vix::websocket::Session &session,
            const std::string &type,
            const vix::json::kvs &payload)
        {
          (void)payload;

          if (type == "room.join")
          {
            server_->join_room(session, "pico");
            session.send_text("joined room pico");

            (void)events_->record(
                "ws",
                "ws.room.join",
                "{\"room\":\"pico\"}");

            return;
          }

          if (type == "broadcast")
          {
            server_->broadcast_room_text(
                "pico",
                "pico broadcast message");

            (void)events_->record(
                "ws",
                "ws.room.broadcast",
                "{\"room\":\"pico\"}");

            return;
          }
        });

    server_->on_close(
        [this](vix::websocket::Session &session)
        {
          (void)session;

          using namespace vix::json;

          const auto payload = o(
              "event", "close",
              "endpoint", "/ws");

          (void)events_->record(
              "ws",
              "ws.close",
              dumps_compact(payload));

          vix::log::info("Pico WebSocket client disconnected");
        });

    server_->on_error(
        [this](
            vix::websocket::Session &session,
            const std::string &error)
        {
          (void)session;

          using namespace vix::json;

          const auto payload = o(
              "error", error);

          (void)events_->record(
              "ws",
              "ws.error",
              dumps_compact(payload));

          vix::log::error("Pico WebSocket error: {}", error);
        });
  }
}
