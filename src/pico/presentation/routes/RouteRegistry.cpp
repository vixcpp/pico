/**
 * @file RouteRegistry.cpp
 * @brief Route registration implementation for the pico backend.
 */

#include <pico/presentation/routes/RouteRegistry.hpp>
#include <pico/presentation/controllers/HomeController.hpp>
#include <pico/presentation/controllers/HealthController.hpp>

#include <vix.hpp>

namespace pico::presentation::routes
{
  void RouteRegistry::register_all(vix::App &app)
  {
    controllers::HomeController::register_routes(app);
    controllers::HealthController::register_routes(app);
  }
} // namespace pico::presentation::routes
