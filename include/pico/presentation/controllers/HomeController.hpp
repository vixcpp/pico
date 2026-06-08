/**
 *
 *  @file HomeController.hpp
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

#ifndef PICO_PRESENTATION_CONTROLLERS_HOME_CONTROLLER_HPP
#define PICO_PRESENTATION_CONTROLLERS_HOME_CONTROLLER_HPP

namespace vix
{
  class App;
}

namespace pico::presentation::controllers
{
  /**
   * @brief Registers public home routes for the backend API.
   *
   * HomeController owns lightweight routes that are useful for verifying
   * that the generated backend is reachable and correctly wired.
   */
  class HomeController
  {
  public:
    /**
     * @brief Registers home routes on the given Vix application.
     *
     * @param app Target application receiving the routes.
     */
    static void register_routes(vix::App &app);
  };
}

#endif // PICO_PRESENTATION_CONTROLLERS_HOME_CONTROLLER_HPP
