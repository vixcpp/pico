/**
 * @file MiddlewareRegistry.hpp
 * @brief Middleware registry for the pico backend.
 */

#ifndef VIX_GENERATED_BACKEND_PICO_MIDDLEWARE_REGISTRY_HPP
#define VIX_GENERATED_BACKEND_PICO_MIDDLEWARE_REGISTRY_HPP

namespace vix
{
  class App;
}

namespace pico::presentation::middleware
{
  /**
   * @brief Central registry for application middleware.
   *
   * MiddlewareRegistry keeps the middleware stack in one place so the
   * application bootstrap can stay small and focused on startup flow.
   */
  class MiddlewareRegistry
  {
  public:
    /**
     * @brief Register all middleware on the given Vix application.
     *
     * @param app Target application receiving the middleware stack.
     */
    static void register_all(vix::App &app);
  };
} // namespace pico::presentation::middleware

#endif // VIX_GENERATED_BACKEND_PICO_MIDDLEWARE_REGISTRY_HPP
