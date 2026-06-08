/**
 *
 *  @file HealthController.hpp
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

#ifndef PICO_PRESENTATION_CONTROLLERS_HEALTH_CONTROLLER_HPP
#define PICO_PRESENTATION_CONTROLLERS_HEALTH_CONTROLLER_HPP

namespace vix
{
  class App;
}

namespace pico::presentation::controllers
{
  /**
   * @brief Registers health check routes for the backend application.
   *
   * HealthController exposes routes intended for local checks, reverse
   * proxies, deployment scripts, and production monitoring.
   */
  class HealthController
  {
  public:
    /**
     * @brief Registers health check routes on the given Vix application.
     *
     * @param app Target application receiving the routes.
     */
    static void register_routes(vix::App &app);
  };
}

#endif // PICO_PRESENTATION_CONTROLLERS_HEALTH_CONTROLLER_HPP
