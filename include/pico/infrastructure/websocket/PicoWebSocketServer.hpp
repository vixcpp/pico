/**
 *
 *  @file PicoWebSocketServer.hpp
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

#ifndef PICO_INFRASTRUCTURE_WEBSOCKET_PICO_WEB_SOCKET_SERVER_HPP
#define PICO_INFRASTRUCTURE_WEBSOCKET_PICO_WEB_SOCKET_SERVER_HPP

#include <memory>

#include <pico/application/services/EventService.hpp>
#include <pico/application/services/RuntimeStatusService.hpp>

namespace vix::config
{
  class Config;
}

namespace vix::executor
{
  class RuntimeExecutor;
}

namespace vix::websocket
{
  class Server;
}

namespace pico::infrastructure::websocket
{
  /**
   * @brief Builds and configures Pico's WebSocket server.
   *
   * PicoWebSocketServer owns the WebSocket server object and its executor,
   * but it does not start or stop the runtime directly.
   *
   * The final lifecycle is handled by Vix AttachedRuntime through
   * vix::run_http_and_ws(...).
   */
  class PicoWebSocketServer
  {
  public:
    /**
     * @brief Creates and configures Pico's WebSocket server.
     *
     * @param cfg Vix configuration.
     * @param runtime_status Runtime status service.
     * @param events Event service.
     */
    PicoWebSocketServer(
        vix::config::Config &cfg,
        application::services::RuntimeStatusService &runtime_status,
        application::services::EventService &events);

    /**
     * @brief Destroys the configured WebSocket server.
     */
    ~PicoWebSocketServer();

    PicoWebSocketServer(const PicoWebSocketServer &) = delete;
    PicoWebSocketServer &operator=(const PicoWebSocketServer &) = delete;

    PicoWebSocketServer(PicoWebSocketServer &&) noexcept = delete;
    PicoWebSocketServer &operator=(PicoWebSocketServer &&) noexcept = delete;

    /**
     * @brief Returns the underlying Vix WebSocket server.
     */
    [[nodiscard]] vix::websocket::Server &server();

    /**
     * @brief Returns the underlying Vix WebSocket server.
     */
    [[nodiscard]] const vix::websocket::Server &server() const;

    /**
     * @brief Returns the shared WebSocket runtime executor.
     */
    [[nodiscard]] std::shared_ptr<vix::executor::RuntimeExecutor> executor() const;

  private:
    void configure_handlers();

  private:
    application::services::RuntimeStatusService *runtime_status_;
    application::services::EventService *events_;

    std::shared_ptr<vix::executor::RuntimeExecutor> executor_;
    std::unique_ptr<vix::websocket::Server> server_;
  };
}

#endif // PICO_INFRASTRUCTURE_WEBSOCKET_PICO_WEB_SOCKET_SERVER_HPP
