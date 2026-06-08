/**
 *
 *  @file AppBootstrap.hpp
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

#ifndef PICO_APP_APP_BOOTSTRAP_HPP
#define PICO_APP_APP_BOOTSTRAP_HPP

namespace pico::app
{
  /**
   * @brief Owns the startup sequence of the Pico backend application.
   *
   * AppBootstrap keeps main.cpp minimal and centralizes configuration loading,
   * Vix app creation, infrastructure initialization, middleware registration,
   * route registration, static files, templates, and server startup.
   */
  class AppBootstrap
  {
  public:
    /**
     * @brief Creates a default application bootstrap instance.
     */
    AppBootstrap() = default;

    /**
     * @brief Destroys the application bootstrap instance.
     */
    ~AppBootstrap() = default;

    AppBootstrap(const AppBootstrap &) = delete;
    AppBootstrap &operator=(const AppBootstrap &) = delete;

    AppBootstrap(AppBootstrap &&) noexcept = delete;
    AppBootstrap &operator=(AppBootstrap &&) noexcept = delete;

    /**
     * @brief Runs the Pico backend application.
     *
     * @return Process exit code.
     */
    int run();
  };
}

#endif // PICO_APP_APP_BOOTSTRAP_HPP
