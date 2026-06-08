/**
 *
 *  @file JobController.hpp
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

#ifndef PICO_PRESENTATION_CONTROLLERS_JOB_CONTROLLER_HPP
#define PICO_PRESENTATION_CONTROLLERS_JOB_CONTROLLER_HPP

#include <pico/application/services/JobService.hpp>

#include <vix.hpp>

namespace pico::presentation::controllers
{
  /**
   * @brief Registers background job diagnostic routes.
   *
   * JobController exposes small routes that submit work to the Vix ThreadPool
   * through JobService. These routes are used to test futures, metrics, timeout
   * observation, and background execution in production.
   */
  class JobController
  {
  public:
    /**
     * @brief Registers job routes on the Vix application.
     *
     * @param app Vix application.
     * @param jobs Job application service.
     */
    static void register_routes(
        vix::App &app,
        application::services::JobService &jobs);
  };
}

#endif // PICO_PRESENTATION_CONTROLLERS_JOB_CONTROLLER_HPP
