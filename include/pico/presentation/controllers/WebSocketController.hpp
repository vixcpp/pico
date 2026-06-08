/**
 *
 *  @file WebSocketController.hpp
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

#ifndef PICO_PRESENTATION_CONTROLLERS_WEB_SOCKET_CONTROLLER_HPP
#define PICO_PRESENTATION_CONTROLLERS_WEB_SOCKET_CONTROLLER_HPP

#include <pico/application/services/EventService.hpp>
#include <pico/application/services/RuntimeStatusService.hpp>

#include <vix.hpp>

namespace pico::presentation::controllers
{
  /**
   * @brief Registers WebSocket diagnostic HTTP routes.
   *
   * WebSocketController exposes HTTP endpoints that describe Pico's
   * WebSocket runtime state. The actual WebSocket server lifecycle belongs
   * to the application bootstrap layer.
   */
  class WebSocketController
  {
  public:
    /**
     * @brief Registers WebSocket diagnostic routes on the Vix application.
     *
     * @param app Vix application.
     * @param runtime_status Runtime status service.
     * @param events Event service used to record WebSocket diagnostics.
     */
    static void register_routes(
        vix::App &app,
        application::services::RuntimeStatusService &runtime_status,
        application::services::EventService &events);
  };
}

#endif // PICO_PRESENTATION_CONTROLLERS_WEB_SOCKET_CONTROLLER_HPP
