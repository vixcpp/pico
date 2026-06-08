/**
 *
 *  @file KvController.cpp
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

#include <pico/presentation/controllers/KvController.hpp>
#include <pico/support/JsonHelpers.hpp>
#include <string>
#include <vix/json.hpp>

namespace pico::presentation::controllers
{
  namespace
  {
    [[nodiscard]] std::string read_key_param(vix::Request &req)
    {
      return req.param("key");
    }
  }

  void KvController::register_routes(
      vix::App &app,
      infrastructure::kv::PicoKvStore &kv,
      application::services::EventService &events)
  {
    app.post("/api/kv",
             [&kv, &events](vix::Request &req, vix::Response &res)
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

               const auto key = get_or<std::string>(*body, "key", "");
               const auto value = get_or<std::string>(*body, "value", "");

               if (key.empty())
               {
                 res.status(vix::http::UNPROCESSABLE_ENTITY)
                     .json(support::make_error_json(
                         "missing_key",
                         "Field 'key' is required"));
                 return;
               }

               kv.put(key, value);

               const auto payload = o(
                   "key", key,
                   "value", value);

               (void)events.record(
                   "kv",
                   "kv.write",
                   dumps_compact(payload));

               res.status(vix::http::CREATED)
                   .json(o(
                       "ok", true,
                       "key", key,
                       "value", value));
             });

    app.post("/api/kv/read",
             [&kv](vix::Request &req, vix::Response &res)
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

               const auto key = get_or<std::string>(*body, "key", "");

               if (key.empty())
               {
                 res.status(vix::http::UNPROCESSABLE_ENTITY)
                     .json(support::make_error_json(
                         "missing_key",
                         "Field 'key' is required"));
                 return;
               }

               const auto value = kv.get(key);

               if (!value.has_value())
               {
                 res.status(vix::http::NOT_FOUND)
                     .json(support::make_error_json(
                         "not_found",
                         "KV key was not found"));
                 return;
               }

               res.json(o(
                   "ok", true,
                   "key", key,
                   "value", *value));
             });

    app.get("/api/kv/:key",
            [&kv](vix::Request &req, vix::Response &res)
            {
              using namespace vix::json;

              const auto key = read_key_param(req);

              if (key.empty())
              {
                res.status(vix::http::UNPROCESSABLE_ENTITY)
                    .json(support::make_error_json(
                        "missing_key",
                        "Route parameter 'key' is required"));
                return;
              }

              const auto value = kv.get(key);

              if (!value.has_value())
              {
                res.status(vix::http::NOT_FOUND)
                    .json(support::make_error_json(
                        "not_found",
                        "KV key was not found"));
                return;
              }

              res.json(o(
                  "ok", true,
                  "key", key,
                  "value", *value));
            });

    app.del("/api/kv/:key",
            [&kv, &events](vix::Request &req, vix::Response &res)
            {
              using namespace vix::json;

              const auto key = read_key_param(req);

              if (key.empty())
              {
                res.status(vix::http::UNPROCESSABLE_ENTITY)
                    .json(support::make_error_json(
                        "missing_key",
                        "Route parameter 'key' is required"));
                return;
              }

              kv.erase(key);

              const auto payload = o(
                  "key", key);

              (void)events.record(
                  "kv",
                  "kv.delete",
                  dumps_compact(payload));

              res.json(o(
                  "ok", true,
                  "deleted", true,
                  "key", key));
            });
  }
}
