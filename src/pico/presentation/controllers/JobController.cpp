/**
 *
 *  @file JobController.cpp
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

#include <pico/presentation/controllers/JobController.hpp>
#include <pico/support/JsonHelpers.hpp>
#include <string>
#include <vix/json.hpp>

namespace pico::presentation::controllers
{
  void JobController::register_routes(
      vix::App &app,
      application::services::JobService &jobs)
  {
    app.post("/api/jobs",
             [&jobs](vix::Request &req, vix::Response &res)
             {
               using namespace vix::json;

               const auto body = try_loads(req.body());

               if (!body.has_value())
               {
                 res.status(vix::http::BAD_REQUEST)
                     .json(support::make_error_json(
                         "invalid_json",
                         "Invalid JSON body"));
                 return;
               }

               const auto name = get_or<std::string>(*body, "name", "test");
               const auto value = get_or<int>(*body, "value", 21);

               try
               {
                 res.status(vix::http::CREATED)
                     .json(jobs.run_job(name, value));
               }
               catch (const std::exception &error)
               {
                 res.status(vix::http::INTERNAL_ERROR)
                     .json(support::make_error_json(
                         "job_failed",
                         error.what()));
               }
             });

    app.post("/api/jobs/slow",
             [&jobs](vix::Request &, vix::Response &res)
             {
               try
               {
                 res.status(vix::http::CREATED)
                     .json(jobs.run_slow_job());
               }
               catch (const std::exception &error)
               {
                 res.status(vix::http::INTERNAL_ERROR)
                     .json(support::make_error_json(
                         "slow_job_failed",
                         error.what()));
               }
             });
  }
}
