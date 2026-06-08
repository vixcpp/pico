/**
 *
 *  @file JobService.hpp
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

#ifndef PICO_APPLICATION_SERVICES_JOB_SERVICE_HPP
#define PICO_APPLICATION_SERVICES_JOB_SERVICE_HPP

#include <string>

#include <pico/application/services/EventService.hpp>
#include <pico/infrastructure/kv/PicoKvStore.hpp>
#include <pico/infrastructure/threadpool/PicoThreadPool.hpp>

#include <vix/json.hpp>

namespace pico::application::services
{
  /**
   * @brief Application service for Pico background job diagnostics.
   *
   * JobService runs small worker-pool tasks through PicoThreadPool and records
   * their result in KV and SQLite events. It is intentionally small because Pico
   * is a production test app, not a full job queue.
   */
  class JobService
  {
  public:
    /**
     * @brief Creates a job service.
     *
     * @param pool Background thread pool wrapper.
     * @param kv Durable KV store used for counters.
     * @param events Event service used to record job diagnostics.
     */
    JobService(
        infrastructure::threadpool::PicoThreadPool &pool,
        infrastructure::kv::PicoKvStore &kv,
        EventService &events);

    JobService(const JobService &) = delete;
    JobService &operator=(const JobService &) = delete;

    JobService(JobService &&) noexcept = default;
    JobService &operator=(JobService &&) noexcept = default;

    ~JobService() = default;

    /**
     * @brief Runs a simple diagnostic job.
     *
     * @param name Job name.
     * @param value Input value.
     * @return JSON payload describing the result.
     */
    [[nodiscard]] vix::json::Json run_job(
        const std::string &name,
        int value);

    /**
     * @brief Runs a deliberately slow job to test timeout observation.
     *
     * @return JSON payload describing the timeout status.
     */
    [[nodiscard]] vix::json::Json run_slow_job();

  private:
    infrastructure::threadpool::PicoThreadPool *pool_;
    infrastructure::kv::PicoKvStore *kv_;
    EventService *events_;
  };
}

#endif // PICO_APPLICATION_SERVICES_JOB_SERVICE_HPP
