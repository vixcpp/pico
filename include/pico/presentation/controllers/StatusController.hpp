/**
 *
 *  @file StatusController.hpp
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

#ifndef PICO_PRESENTATION_CONTROLLERS_STATUS_CONTROLLER_HPP
#define PICO_PRESENTATION_CONTROLLERS_STATUS_CONTROLLER_HPP

#include <pico/application/services/RuntimeStatusService.hpp>

#include <vix.hpp>

namespace pico::presentation::controllers
{
  /**
   * @brief Registers runtime status and ThreadPool diagnostics routes.
   *
   * StatusController exposes small JSON endpoints used to observe Pico in
   * local development and production.
   */
  class StatusController
  {
  public:
    /**
     * @brief Registers status routes on the Vix application.
     *
     * @param app Vix application.
     * @param runtime_status Runtime status service.
     */
    static void register_routes(
        vix::App &app,
        application::services::RuntimeStatusService &runtime_status);
  };
}

#endif // PICO_PRESENTATION_CONTROLLERS_STATUS_CONTROLLER_HPP
