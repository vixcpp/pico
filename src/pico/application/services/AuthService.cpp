/**
 *
 *  @file AuthService.cpp
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

#include <pico/application/services/AuthService.hpp>

namespace pico::application::services
{
  AuthService::AuthService(
      vix::db::Database &database,
      EventService &events)
      : auth_(
            rix.auth.database(
                database,
                []()
                {
                  auto config = rix.auth.config.production();

                  config.set_require_email_verification(false);
                  config.set_issuer("pico");

                  return config;
                }())),
        events_(&events)
  {
  }

  vix::json::Json AuthService::register_user(
      const std::string &email,
      const std::string &password)
  {
    using namespace vix::json;

    auto registered = auth_.register_user({email, password});

    if (registered.failed())
    {
      const auto payload = o(
          "email", email,
          "error", std::string{rix.auth.error.to_string(registered.error())});

      (void)events_->record(
          "auth",
          "auth.register.failed",
          dumps_compact(payload));

      return make_auth_error_json(registered.error());
    }

    const auto &user = registered.value();

    const auto payload = o(
        "user_id", user.id(),
        "email", user.email());

    (void)events_->record(
        "auth",
        "auth.user.registered",
        dumps_compact(payload));

    return o(
        "ok", true,
        "user", o("id", user.id(), "email", user.email()));
  }

  vix::json::Json AuthService::login(
      const std::string &email,
      const std::string &password)
  {
    using namespace vix::json;

    auto logged_in = auth_.login({email, password});

    if (logged_in.failed())
    {
      const auto payload = o(
          "email", email,
          "error", std::string{rix.auth.error.to_string(logged_in.error())});

      (void)events_->record(
          "auth",
          "auth.login.failed",
          dumps_compact(payload));

      return make_auth_error_json(logged_in.error());
    }

    const auto &result = logged_in.value();

    const auto payload = o(
        "user_id", result.user.id(),
        "email", result.user.email());

    (void)events_->record(
        "auth",
        "auth.login.succeeded",
        dumps_compact(payload));

    return o(
        "ok", true,
        "user", o("id", result.user.id(), "email", result.user.email()),
        "session", o("id", result.session.id(), "expires_at", result.session.expires_at()),
        "token", o("issuer", result.token.issuer(), "issued_at", result.token.issued_at(), "expires_at", result.token.expires_at()));
  }

  vix::json::Json AuthService::check_session(
      const std::string &session_id)
  {
    using namespace vix::json;

    auto session = auth_.authenticate_session(session_id);

    if (session.failed())
    {
      const auto payload = o(
          "error", std::string{rix.auth.error.to_string(session.error())});

      (void)events_->record(
          "auth",
          "auth.session.rejected",
          dumps_compact(payload));

      return make_auth_error_json(session.error());
    }

    const auto &value = session.value();

    const auto payload = o(
        "user_id", value.user_id());

    (void)events_->record(
        "auth",
        "auth.session.authenticated",
        dumps_compact(payload));

    return o(
        "ok", true,
        "session", o("id", value.id(), "user_id", value.user_id(), "expires_at", value.expires_at(), "last_seen_at", value.last_seen_at()));
  }

  vix::json::Json AuthService::logout(
      const std::string &session_id)
  {
    using namespace vix::json;

    auto logged_out = auth_.logout(session_id);

    if (logged_out.failed())
    {
      const auto payload = o(
          "error", std::string{rix.auth.error.to_string(logged_out.error())});

      (void)events_->record(
          "auth",
          "auth.logout.failed",
          dumps_compact(payload));

      return make_auth_error_json(logged_out.error());
    }

    (void)events_->record(
        "auth",
        "auth.logout.succeeded",
        "{}");

    return o(
        "ok", true,
        "logged_out", true);
  }

  vix::json::Json AuthService::delete_account(
      const std::string &session_id)
  {
    using namespace vix::json;

    auto session = auth_.authenticate_session(session_id);

    if (session.failed())
    {
      const auto payload = o(
          "error", std::string{rix.auth.error.to_string(session.error())});

      (void)events_->record(
          "auth",
          "auth.account.delete.rejected",
          dumps_compact(payload));

      return make_auth_error_json(session.error());
    }

    const auto user_id = session.value().user_id();

    auto logged_out = auth_.logout_user(user_id);

    if (logged_out.failed())
    {
      const auto payload = o(
          "user_id", user_id,
          "error", std::string{rix.auth.error.to_string(logged_out.error())});

      (void)events_->record(
          "auth",
          "auth.account.delete.logout_failed",
          dumps_compact(payload));

      return make_auth_error_json(logged_out.error());
    }

    auto removed = auth_.users().remove_by_id(user_id);

    if (removed.failed())
    {
      const auto payload = o(
          "user_id", user_id,
          "error", std::string{rix.auth.error.to_string(removed.error())});

      (void)events_->record(
          "auth",
          "auth.account.delete.failed",
          dumps_compact(payload));

      return make_auth_error_json(removed.error());
    }

    (void)events_->record(
        "auth",
        "auth.account.deleted",
        dumps_compact(o(
            "user_id", user_id)));

    return o(
        "ok", true,
        "deleted", true);
  }

  vix::json::Json AuthService::make_auth_error_json(
      const rixlib::auth::AuthError &error) const
  {
    using namespace vix::json;

    return o(
        "ok", false,
        "error", o("code", std::string{rix.auth.error.to_string(error)}, "message", error.message()));
  }
}
