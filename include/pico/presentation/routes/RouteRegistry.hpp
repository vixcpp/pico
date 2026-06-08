/**
 * @file RouteRegistry.hpp
 * @brief Route registry for the pico backend.
 */

#ifndef VIX_GENERATED_BACKEND_PICO_ROUTE_REGISTRY_HPP
#define VIX_GENERATED_BACKEND_PICO_ROUTE_REGISTRY_HPP

namespace vix
{
  class App;
}

namespace pico::presentation::routes
{
  /**
   * @brief Central registry for application routes.
   *
   * RouteRegistry keeps route registration grouped in one place so the
   * application bootstrap does not need to know about individual controllers.
   */
  class RouteRegistry
  {
  public:
    /**
     * @brief Register all application routes on the given Vix application.
     *
     * @param app Target application receiving the route declarations.
     */
    static void register_all(vix::App &app);
  };
} // namespace pico::presentation::routes

#endif // VIX_GENERATED_BACKEND_PICO_ROUTE_REGISTRY_HPP
