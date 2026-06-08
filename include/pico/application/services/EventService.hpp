/**
 *
 *  @file EventService.hpp
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

#ifndef PICO_APPLICATION_SERVICES_EVENT_SERVICE_HPP
#define PICO_APPLICATION_SERVICES_EVENT_SERVICE_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include <pico/domain/models/Event.hpp>
#include <pico/infrastructure/database/EventRepository.hpp>

namespace pico::application::services
{
  /**
   * @brief Application service for recording and reading Pico events.
   *
   * EventService is the application layer between HTTP/WebSocket controllers
   * and the SQLite event repository. It keeps event creation rules outside
   * the presentation layer.
   */
  class EventService
  {
  public:
    /**
     * @brief Creates an event service.
     *
     * @param repository Event repository used for persistence.
     */
    explicit EventService(
        infrastructure::database::EventRepository &repository);

    EventService(const EventService &) = delete;
    EventService &operator=(const EventService &) = delete;

    EventService(EventService &&) noexcept = default;
    EventService &operator=(EventService &&) noexcept = default;

    ~EventService() = default;

    /**
     * @brief Ensures the event storage schema exists.
     */
    void initialize();

    /**
     * @brief Records a Pico runtime event.
     *
     * @param source Event source, for example http, ws, kv, job, system.
     * @param type Event type, for example request.received.
     * @param payload Compact JSON or text payload.
     * @return Inserted event id.
     */
    [[nodiscard]] std::int64_t record(
        const std::string &source,
        const std::string &type,
        const std::string &payload);

    /**
     * @brief Returns the latest stored events.
     *
     * @param limit Maximum number of events.
     * @return Event list ordered from newest to oldest.
     */
    [[nodiscard]] std::vector<domain::models::Event> latest(
        std::size_t limit) const;

    /**
     * @brief Counts all stored events.
     *
     * @return Total event count.
     */
    [[nodiscard]] std::int64_t count() const;

  private:
    infrastructure::database::EventRepository *repository_;
  };
}

#endif // PICO_APPLICATION_SERVICES_EVENT_SERVICE_HPP
