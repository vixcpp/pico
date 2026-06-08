/**
 * @file HealthController.cpp
 * @brief Health route implementation for the pico backend.
 */

#include <pico/presentation/controllers/HealthController.hpp>

#include <vix.hpp>

namespace pico::presentation::controllers
{
  void HealthController::register_routes(vix::App &app)
  {
    app.get("/health", [](vix::Request &req, vix::Response &res)
    {
      (void)req;

      res.json({
        "ok", true,
        "status", "ok",
        "service", "pico"
      });
    });

    app.get("/api/health", [](vix::Request &req, vix::Response &res)
    {
      (void)req;

      res.json({
        "ok", true,
        "status", "ok",
        "service", "pico",
        "api", true
      });
    });
  }
} // namespace pico::presentation::controllers
