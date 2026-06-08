/**
 *
 *  @file KvController.hpp
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

#ifndef PICO_PRESENTATION_CONTROLLERS_KV_CONTROLLER_HPP
#define PICO_PRESENTATION_CONTROLLERS_KV_CONTROLLER_HPP

#include <pico/application/services/EventService.hpp>
#include <pico/infrastructure/kv/PicoKvStore.hpp>

#include <vix.hpp>

namespace pico::presentation::controllers
{
  /**
   * @brief Registers durable KV diagnostic routes.
   *
   * KvController exposes small endpoints that write, read, and delete values
   * from the Vix KV store. These routes are used to test persistence,
   * tombstones, recovery, and JSON request handling in production.
   */
  class KvController
  {
  public:
    /**
     * @brief Registers KV routes on the Vix application.
     *
     * @param app Vix application.
     * @param kv Durable Pico KV store.
     * @param events Event service used to record KV actions.
     */
    static void register_routes(
        vix::App &app,
        infrastructure::kv::PicoKvStore &kv,
        application::services::EventService &events);
  };
}

#endif // PICO_PRESENTATION_CONTROLLERS_KV_CONTROLLER_HPP
