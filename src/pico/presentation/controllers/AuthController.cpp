/**
 *
 *  @file AuthController.cpp
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

#include <pico/presentation/controllers/AuthController.hpp>
#include <pico/support/JsonHelpers.hpp>

#include <string>

#include <vix/json.hpp>

namespace pico::presentation::controllers
{
  namespace
  {
    [[nodiscard]] bool response_ok(const vix::json::Json &payload)
    {
      return payload.value("ok", false);
    }

    [[nodiscard]] std::string read_string_field(
        const vix::json::Json &body,
        const std::string &field)
    {
      return vix::json::get_or<std::string>(body, field, "");
    }

    [[nodiscard]] bool read_json_body(
        vix::Request &req,
        vix::Response &res,
        vix::json::Json &body)
    {
      const auto parsed = vix::json::try_loads(req.body());

      if (!parsed.has_value())
      {
        res.status(vix::http::BAD_REQUEST)
            .json(support::make_error_json(
                "invalid_json",
                "Invalid JSON body"));

        return false;
      }

      body = *parsed;
      return true;
    }

    [[nodiscard]] bool require_field(
        vix::Response &res,
        const std::string &value,
        const std::string &field)
    {
      if (!value.empty())
      {
        return true;
      }

      res.status(vix::http::UNPROCESSABLE_ENTITY)
          .json(support::make_error_json(
              "missing_" + field,
              "Field '" + field + "' is required"));

      return false;
    }

    void send_auth_result(
        vix::Response &res,
        const vix::json::Json &payload)
    {
      if (!response_ok(payload))
      {
        res.status(vix::http::UNPROCESSABLE_ENTITY)
            .json(payload);

        return;
      }

      res.json(payload);
    }
  }

  void AuthController::register_routes(
      vix::App &app,
      application::services::AuthService &auth)
  {
    app.post("/api/auth/register",
             [&auth](vix::Request &req, vix::Response &res)
             {
               vix::json::Json body{};

               if (!read_json_body(req, res, body))
               {
                 return;
               }

               const auto email = read_string_field(body, "email");
               const auto password = read_string_field(body, "password");

               if (!require_field(res, email, "email"))
               {
                 return;
               }

               if (!require_field(res, password, "password"))
               {
                 return;
               }

               const auto payload = auth.register_user(email, password);

               if (!response_ok(payload))
               {
                 res.status(vix::http::UNPROCESSABLE_ENTITY)
                     .json(payload);

                 return;
               }

               res.status(vix::http::CREATED)
                   .json(payload);
             });

    app.post("/api/auth/login",
             [&auth](vix::Request &req, vix::Response &res)
             {
               vix::json::Json body{};

               if (!read_json_body(req, res, body))
               {
                 return;
               }

               const auto email = read_string_field(body, "email");
               const auto password = read_string_field(body, "password");

               if (!require_field(res, email, "email"))
               {
                 return;
               }

               if (!require_field(res, password, "password"))
               {
                 return;
               }

               send_auth_result(
                   res,
                   auth.login(email, password));
             });

    app.post("/api/auth/check",
             [&auth](vix::Request &req, vix::Response &res)
             {
               vix::json::Json body{};

               if (!read_json_body(req, res, body))
               {
                 return;
               }

               const auto session_id = read_string_field(body, "session_id");

               if (!require_field(res, session_id, "session_id"))
               {
                 return;
               }

               send_auth_result(
                   res,
                   auth.check_session(session_id));
             });

    app.post("/api/auth/logout",
             [&auth](vix::Request &req, vix::Response &res)
             {
               vix::json::Json body{};

               if (!read_json_body(req, res, body))
               {
                 return;
               }

               const auto session_id = read_string_field(body, "session_id");

               if (!require_field(res, session_id, "session_id"))
               {
                 return;
               }

               send_auth_result(
                   res,
                   auth.logout(session_id));
             });

    app.del("/api/auth/account",
            [&auth](vix::Request &req, vix::Response &res)
            {
              vix::json::Json body{};

              if (!read_json_body(req, res, body))
              {
                return;
              }

              const auto session_id = read_string_field(body, "session_id");

              if (!require_field(res, session_id, "session_id"))
              {
                return;
              }

              send_auth_result(
                  res,
                  auth.delete_account(session_id));
            });
  }
}
