/**
 *
 *  @file Event.hpp
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

#ifndef PICO_DOMAIN_MODELS_EVENT_HPP
#define PICO_DOMAIN_MODELS_EVENT_HPP

#include <cstdint>
#include <string>

namespace pico::domain::models
{
  /**
   * @brief Represents a small runtime event recorded by Pico.
   *
   * Events are used to observe HTTP requests, WebSocket messages,
   * background jobs, KV actions, database writes, and production diagnostics.
   */
  struct Event
  {
    std::int64_t id{0};
    std::string source{};
    std::string type{};
    std::string payload{};
    std::string created_at{};

    /**
     * @brief Checks whether the event has a database id.
     *
     * @return true when the id is greater than zero.
     */
    [[nodiscard]] bool has_id() const noexcept
    {
      return id > 0;
    }

    /**
     * @brief Checks whether the event contains the minimum required fields.
     *
     * @return true when source and type are not empty.
     */
    [[nodiscard]] bool valid() const noexcept
    {
      return !source.empty() && !type.empty();
    }
  };
}

#endif // PICO_DOMAIN_MODELS_EVENT_HPP
