/**
 * @file HttpResponses.hpp
 * @brief JSON response helpers for the pico backend.
 */

#ifndef VIX_GENERATED_BACKEND_PICO_HTTP_RESPONSES_HPP
#define VIX_GENERATED_BACKEND_PICO_HTTP_RESPONSES_HPP

#include <string>

#include <vix.hpp>

namespace pico::support
{
  /**
   * @brief Write a standard JSON error response.
   *
   * @param res HTTP response wrapper.
   * @param status HTTP status code to send.
   * @param code Machine-readable error code.
   * @param message Human-readable error message.
   */
  void json_error(
      vix::Response &res,
      int status,
      const std::string &code,
      const std::string &message);

  /**
   * @brief Write a standard successful JSON response with data.
   *
   * @param res HTTP response wrapper.
   * @param data JSON payload to place under the data field.
   */
  void json_ok(
      vix::Response &res,
      const vix::json::Json &data);

  /**
   * @brief Write a standard successful JSON response with a message.
   *
   * @param res HTTP response wrapper.
   * @param message Message to send to the client.
   */
  void json_message(
      vix::Response &res,
      const std::string &message);
} // namespace pico::support

#endif // VIX_GENERATED_BACKEND_PICO_HTTP_RESPONSES_HPP
