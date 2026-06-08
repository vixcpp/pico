/**
 *
 *  @file EventRepository.hpp
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

#ifndef PICO_INFRASTRUCTURE_DATABASE_EVENT_REPOSITORY_HPP
#define PICO_INFRASTRUCTURE_DATABASE_EVENT_REPOSITORY_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include <pico/domain/models/Event.hpp>
#include <pico/infrastructure/database/DatabaseProvider.hpp>

namespace pico::infrastructure::database
{
  /**
   * @brief Stores and reads Pico runtime events from SQLite.
   *
   * EventRepository is used by application services to persist HTTP,
   * WebSocket, KV, job, and diagnostics events without exposing SQL
   * to the presentation layer.
   */
  class EventRepository
  {
  public:
    /**
     * @brief Creates an event repository.
     *
     * @param provider Open database provider used by the repository.
     */
    explicit EventRepository(DatabaseProvider &provider);

    EventRepository(const EventRepository &) = delete;
    EventRepository &operator=(const EventRepository &) = delete;

    EventRepository(EventRepository &&) noexcept = default;
    EventRepository &operator=(EventRepository &&) noexcept = default;

    ~EventRepository() = default;

    /**
     * @brief Creates the events table when it does not already exist.
     */
    void ensure_schema();

    /**
     * @brief Inserts a runtime event.
     *
     * @param source Event source, for example http, ws, kv, job, system.
     * @param type Event type, for example health.check or ws.message.
     * @param payload Compact JSON or text payload.
     * @return Inserted event id when available, otherwise zero.
     */
    [[nodiscard]] std::int64_t create(
        const std::string &source,
        const std::string &type,
        const std::string &payload);

    /**
     * @brief Returns the latest runtime events.
     *
     * @param limit Maximum number of events to return.
     * @return Event list ordered from newest to oldest.
     */
    [[nodiscard]] std::vector<domain::models::Event> latest(
        std::size_t limit) const;

    /**
     * @brief Counts all stored events.
     *
     * @return Total number of stored events.
     */
    [[nodiscard]] std::int64_t count() const;

  private:
    DatabaseProvider *provider_;
  };
}

#endif // PICO_INFRASTRUCTURE_DATABASE_EVENT_REPOSITORY_HPP
