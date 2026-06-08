/**
 *
 *  @file EventController.hpp
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

#ifndef PICO_PRESENTATION_CONTROLLERS_EVENT_CONTROLLER_HPP
#define PICO_PRESENTATION_CONTROLLERS_EVENT_CONTROLLER_HPP

#include <pico/application/services/EventService.hpp>

#include <vix.hpp>

namespace pico::presentation::controllers
{
  /**
   * @brief Registers event diagnostic routes.
   *
   * EventController exposes the small HTTP API used to record and inspect
   * Pico runtime events stored in SQLite.
   */
  class EventController
  {
  public:
    /**
     * @brief Registers event routes on the Vix application.
     *
     * @param app Vix application.
     * @param events Event application service.
     */
    static void register_routes(
        vix::App &app,
        application::services::EventService &events);
  };
}

#endif // PICO_PRESENTATION_CONTROLLERS_EVENT_CONTROLLER_HPP
