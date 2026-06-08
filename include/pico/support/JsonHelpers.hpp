/**
 *
 *  @file JsonHelpers.hpp
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

#ifndef PICO_SUPPORT_JSON_HELPERS_HPP
#define PICO_SUPPORT_JSON_HELPERS_HPP

#include <pico/domain/models/Event.hpp>

#include <string>
#include <vector>

#include <vix/json.hpp>

namespace pico::support
{
  /**
   * @brief Creates a compact JSON error response.
   *
   * @param code Stable error code.
   * @param message Human-readable error message.
   * @return JSON object containing the error payload.
   */
  [[nodiscard]] vix::json::Json make_error_json(
      const std::string &code,
      const std::string &message);

  /**
   * @brief Creates a compact JSON success response.
   *
   * @param message Human-readable success message.
   * @return JSON object containing the success payload.
   */
  [[nodiscard]] vix::json::Json make_ok_json(const std::string &message);

  /**
   * @brief Converts a Pico event to JSON.
   *
   * @param event Event model.
   * @return JSON object representing the event.
   */
  [[nodiscard]] vix::json::Json event_to_json(
      const domain::models::Event &event);

  /**
   * @brief Converts a list of Pico events to a JSON array.
   *
   * @param events Event list.
   * @return JSON array containing all events.
   */
  [[nodiscard]] vix::json::Json events_to_json(
      const std::vector<domain::models::Event> &events);
}

#endif // PICO_SUPPORT_JSON_HELPERS_HPP
