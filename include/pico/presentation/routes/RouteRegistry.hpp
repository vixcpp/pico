/**
 *
 *  @file RouteRegistry.hpp
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

#ifndef PICO_PRESENTATION_ROUTES_ROUTE_REGISTRY_HPP
#define PICO_PRESENTATION_ROUTES_ROUTE_REGISTRY_HPP

#include <pico/application/services/EventService.hpp>
#include <pico/application/services/JobService.hpp>
#include <pico/application/services/RuntimeStatusService.hpp>
#include <pico/infrastructure/kv/PicoKvStore.hpp>

namespace vix
{
  class App;
}

namespace pico::presentation::routes
{
  /**
   * @brief Central registry for Pico HTTP routes.
   *
   * RouteRegistry keeps route registration grouped in one place so the
   * application bootstrap does not need to know every controller directly.
   */
  class RouteRegistry
  {
  public:
    /**
     * @brief Registers all application routes on the given Vix application.
     *
     * @param app Target Vix application.
     * @param runtime_status Runtime status service.
     * @param kv Durable KV store.
     * @param events Event service.
     * @param jobs Job service.
     */
    static void register_all(
        vix::App &app,
        application::services::RuntimeStatusService &runtime_status,
        infrastructure::kv::PicoKvStore &kv,
        application::services::EventService &events,
        application::services::JobService &jobs);
  };
}

#endif // PICO_PRESENTATION_ROUTES_ROUTE_REGISTRY_HPP
