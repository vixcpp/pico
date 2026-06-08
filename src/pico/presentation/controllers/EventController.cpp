/**
 *
 *  @file EventController.cpp
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

#include <pico/presentation/controllers/EventController.hpp>
#include <pico/support/JsonHelpers.hpp>
#include <string>
#include <vix/json.hpp>
#include <rix.hpp>

namespace pico::presentation::controllers
{
  void EventController::register_routes(
      vix::App &app,
      application::services::EventService &events)
  {
    app.get("/api/events",
            [&events](vix::Request &, vix::Response &res)
            {
              using namespace vix::json;

              const auto latest_events = events.latest(25);

              res.json(o(
                  "ok", true,
                  "count", latest_events.size(),
                  "events", support::events_to_json(latest_events)));
            });

    app.post("/api/events",
             [&events](vix::Request &req, vix::Response &res)
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

               const auto source = get_or<std::string>(*body, "source", "http");
               const auto type = get_or<std::string>(*body, "type", "");
               const auto payload = get_or<std::string>(*body, "payload", "{}");

               if (type.empty())
               {
                 res.status(vix::http::UNPROCESSABLE_ENTITY)
                     .json(support::make_error_json(
                         "missing_type",
                         "Field 'type' is required"));
                 return;
               }

               const auto id = events.record(source, type, payload);

               res.status(vix::http::CREATED)
                   .json(o(
                       "ok", true,
                       "id", id,
                       "source", source,
                       "type", type,
                       "payload", payload));
             });

    app.get("/api/events.csv",
            [&events](vix::Request &, vix::Response &res)
            {
              const auto latest_events = events.latest(100);

              rixlib::csv::Table table{};
              table.push_back({"id", "source", "type", "payload", "created_at"});

              for (const auto &event : latest_events)
              {
                table.push_back({
                    std::to_string(event.id),
                    event.source,
                    event.type,
                    event.payload,
                    event.created_at,
                });
              }

              const std::string csv = rix.csv.write(table);

              res.header("Content-Type", "text/csv; charset=utf-8");
              res.header("Content-Disposition", "attachment; filename=\"pico-events.csv\"");
              res.send(csv);
            });
  }
}
