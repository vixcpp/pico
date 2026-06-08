/**
 *
 *  @file MiddlewareRegistry.hpp
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

#ifndef PICO_PRESENTATION_MIDDLEWARE_MIDDLEWARE_REGISTRY_HPP
#define PICO_PRESENTATION_MIDDLEWARE_MIDDLEWARE_REGISTRY_HPP

namespace vix
{
  class App;
}

namespace pico::presentation::middleware
{
  /**
   * @brief Central registry for Pico middleware.
   *
   * MiddlewareRegistry keeps HTTP middleware in one place so the application
   * bootstrap can stay focused on startup, infrastructure, and service wiring.
   */
  class MiddlewareRegistry
  {
  public:
    /**
     * @brief Registers all middleware on the given Vix application.
     *
     * @param app Target Vix application.
     */
    static void register_all(vix::App &app);
  };
}

#endif // PICO_PRESENTATION_MIDDLEWARE_MIDDLEWARE_REGISTRY_HPP
