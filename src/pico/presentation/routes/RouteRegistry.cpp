/**
 *
 *  @file RouteRegistry.cpp
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

#include <pico/presentation/routes/RouteRegistry.hpp>

#include <pico/presentation/controllers/AuthController.hpp>
#include <pico/presentation/controllers/EventController.hpp>
#include <pico/presentation/controllers/HealthController.hpp>
#include <pico/presentation/controllers/HomeController.hpp>
#include <pico/presentation/controllers/JobController.hpp>
#include <pico/presentation/controllers/KvController.hpp>
#include <pico/presentation/controllers/PdfController.hpp>
#include <pico/presentation/controllers/StatusController.hpp>
#include <pico/presentation/controllers/WebSocketController.hpp>

#include <vix.hpp>

namespace pico::presentation::routes
{
  void RouteRegistry::register_all(
      vix::App &app,
      application::services::RuntimeStatusService &runtime_status,
      infrastructure::kv::PicoKvStore &kv,
      application::services::EventService &events,
      application::services::JobService &jobs,
      application::services::AuthService &auth,
      application::services::PdfService &pdfs)
  {
    controllers::HomeController::register_routes(app);
    controllers::HealthController::register_routes(app);

    controllers::StatusController::register_routes(
        app,
        runtime_status);

    controllers::KvController::register_routes(
        app,
        kv,
        events);

    controllers::EventController::register_routes(
        app,
        events);

    controllers::AuthController::register_routes(
        app,
        auth);

    controllers::JobController::register_routes(
        app,
        jobs);

    controllers::PdfController::register_routes(
        app,
        pdfs);

    controllers::WebSocketController::register_routes(
        app,
        runtime_status,
        events);
  }
}
