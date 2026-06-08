/**
 *
 *  @file RuntimeStatusService.hpp
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

#ifndef PICO_APPLICATION_SERVICES_RUNTIME_STATUS_SERVICE_HPP
#define PICO_APPLICATION_SERVICES_RUNTIME_STATUS_SERVICE_HPP

#include <chrono>
#include <cstdint>
#include <string>

#include <pico/application/services/EventService.hpp>
#include <pico/infrastructure/kv/PicoKvStore.hpp>
#include <pico/infrastructure/threadpool/PicoThreadPool.hpp>

#include <vix/json.hpp>

namespace pico::application::services
{
  /**
   * @brief Builds runtime status snapshots for Pico.
   *
   * RuntimeStatusService reads lightweight runtime counters from KV,
   * event totals from SQLite, and worker state from the Vix ThreadPool.
   * It is used by status and health endpoints to expose production
   * diagnostics through a small JSON response.
   */
  class RuntimeStatusService
  {
  public:
    /**
     * @brief Creates a runtime status service.
     *
     * @param kv Durable KV store used for runtime counters.
     * @param pool Background thread pool wrapper.
     * @param events Event service used for event counters.
     */
    RuntimeStatusService(
        infrastructure::kv::PicoKvStore &kv,
        infrastructure::threadpool::PicoThreadPool &pool,
        EventService &events);

    RuntimeStatusService(const RuntimeStatusService &) = delete;
    RuntimeStatusService &operator=(const RuntimeStatusService &) = delete;

    RuntimeStatusService(RuntimeStatusService &&) noexcept = default;
    RuntimeStatusService &operator=(RuntimeStatusService &&) noexcept = default;

    ~RuntimeStatusService() = default;

    /**
     * @brief Marks the application startup in durable KV.
     *
     * @return New boot counter value.
     */
    [[nodiscard]] std::int64_t mark_boot();

    /**
     * @brief Stores a new heartbeat value in durable KV.
     *
     * @param value Heartbeat value.
     */
    void heartbeat(const std::string &value);

    /**
     * @brief Increments the HTTP request counter.
     *
     * @return New HTTP request counter value.
     */
    [[nodiscard]] std::int64_t increment_http_requests();

    /**
     * @brief Increments the WebSocket message counter.
     *
     * @return New WebSocket message counter value.
     */
    [[nodiscard]] std::int64_t increment_ws_messages();

    /**
     * @brief Returns the application uptime in seconds.
     *
     * @return Number of seconds since service construction.
     */
    [[nodiscard]] std::int64_t uptime_seconds() const;

    /**
     * @brief Builds the main runtime status JSON object.
     *
     * @return JSON object containing app, KV, SQLite, and ThreadPool status.
     */
    [[nodiscard]] vix::json::Json status() const;

    /**
     * @brief Builds the health JSON object.
     *
     * @return JSON object for health checks.
     */
    [[nodiscard]] vix::json::Json health() const;

    /**
     * @brief Builds a ThreadPool-only status JSON object.
     *
     * @return JSON object containing ThreadPool metrics and stats.
     */
    [[nodiscard]] vix::json::Json threadpool_status() const;

  private:
    infrastructure::kv::PicoKvStore *kv_;
    infrastructure::threadpool::PicoThreadPool *pool_;
    EventService *events_;
    std::chrono::steady_clock::time_point started_at_;
  };
}

#endif // PICO_APPLICATION_SERVICES_RUNTIME_STATUS_SERVICE_HPP
