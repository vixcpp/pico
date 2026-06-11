/**
 *
 *  @file AuthService.hpp
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

#ifndef PICO_APPLICATION_SERVICES_AUTH_SERVICE_HPP
#define PICO_APPLICATION_SERVICES_AUTH_SERVICE_HPP

#include <string>

#include <pico/application/services/EventService.hpp>

#include <rix.hpp>
#include <vix/db/Database.hpp>
#include <vix/json.hpp>

namespace pico::application::services
{
  /**
   * @brief Application service for rix/auth production diagnostics.
   *
   * AuthService wraps the Rix database-backed auth facade so Pico can test
   * registration, login, session validation, and logout in production without
   * exposing Rix storage details to HTTP controllers.
   */
  class AuthService
  {
  public:
    /**
     * @brief Creates an auth service backed by the Pico database.
     *
     * @param database Vix database facade used by rix/auth stores.
     * @param events Event service used to record auth diagnostics.
     */
    AuthService(
        vix::db::Database &database,
        EventService &events);

    AuthService(const AuthService &) = delete;
    AuthService &operator=(const AuthService &) = delete;

    AuthService(AuthService &&) noexcept = delete;
    AuthService &operator=(AuthService &&) noexcept = delete;

    ~AuthService() = default;

    /**
     * @brief Registers a user through rix/auth.
     *
     * @param email User email.
     * @param password Plain-text password.
     * @return JSON response payload.
     */
    [[nodiscard]] vix::json::Json register_user(
        const std::string &email,
        const std::string &password);

    /**
     * @brief Logs in a user through rix/auth.
     *
     * @param email User email.
     * @param password Plain-text password.
     * @return JSON response payload.
     */
    [[nodiscard]] vix::json::Json login(
        const std::string &email,
        const std::string &password);

    /**
     * @brief Authenticates an existing session id.
     *
     * @param session_id Session id.
     * @return JSON response payload.
     */
    [[nodiscard]] vix::json::Json check_session(
        const std::string &session_id);

    /**
     * @brief Logs out an existing session id.
     *
     * @param session_id Session id.
     * @return JSON response payload.
     */
    [[nodiscard]] vix::json::Json logout(
        const std::string &session_id);

    /**
     * @brief Deletes the authenticated account attached to a session.
     *
     * The session is authenticated first, then all user sessions are revoked,
     * then the user is removed from the auth user store.
     *
     * @param session_id Session id of the authenticated user.
     * @return JSON response payload.
     */
    [[nodiscard]] vix::json::Json delete_account(
        const std::string &session_id);

  private:
    [[nodiscard]] vix::json::Json make_auth_error_json(
        const rixlib::auth::AuthError &error) const;

  private:
    rixlib::auth::ManagedAuth auth_;
    EventService *events_;
  };
}

#endif // PICO_APPLICATION_SERVICES_AUTH_SERVICE_HPP
