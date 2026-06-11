/**
 *
 *  @file AuthController.hpp
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

#ifndef PICO_PRESENTATION_CONTROLLERS_AUTH_CONTROLLER_HPP
#define PICO_PRESENTATION_CONTROLLERS_AUTH_CONTROLLER_HPP

#include <pico/application/services/AuthService.hpp>

#include <vix.hpp>

namespace pico::presentation::controllers
{
  /**
   * @brief Registers rix/auth production diagnostic routes.
   *
   * AuthController exposes small HTTP endpoints that validate rix/auth through
   * Pico using database-backed auth storage.
   */
  class AuthController
  {
  public:
    /**
     * @brief Registers auth diagnostic routes on the Vix application.
     *
     * @param app Vix application.
     * @param auth Auth application service.
     */
    static void register_routes(
        vix::App &app,
        application::services::AuthService &auth);
  };
}

#endif // PICO_PRESENTATION_CONTROLLERS_AUTH_CONTROLLER_HPP
