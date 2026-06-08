/**
 * @file HomeController.cpp
 * @brief Home route implementation for the pico backend.
 */

#include <pico/presentation/controllers/HomeController.hpp>

#include <vix.hpp>

namespace pico::presentation::controllers
{
  void HomeController::register_routes(vix::App &app)
  {
    app.get("/api", [](vix::Request &req, vix::Response &res)
    {
      (void)req;

      res.json({
        "ok", true,
        "service", "pico",
        "message", "Vix backend is running"
      });
    });
  }
} // namespace pico::presentation::controllers
