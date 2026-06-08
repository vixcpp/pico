/**
 * @file HomeController.hpp
 * @brief Home API routes for the pico backend.
 */

#ifndef VIX_GENERATED_BACKEND_PICO_HOME_CONTROLLER_HPP
#define VIX_GENERATED_BACKEND_PICO_HOME_CONTROLLER_HPP

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
     * @brief Register the home routes on the given Vix application.
     *
     * @param app Target application receiving the routes.
     */
    static void register_routes(vix::App &app);
  };
} // namespace pico::presentation::controllers

#endif // VIX_GENERATED_BACKEND_PICO_HOME_CONTROLLER_HPP
