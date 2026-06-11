/**
 *
 *  @file HttpQuery.hpp
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

#ifndef PICO_SUPPORT_HTTP_QUERY_HPP
#define PICO_SUPPORT_HTTP_QUERY_HPP

#include <cstddef>
#include <vix.hpp>

namespace pico::support
{
  /**
   * @brief Represents pagination values parsed from an HTTP request.
   */
  struct Pagination
  {
    std::size_t limit{25};
    std::size_t offset{0};
  };

  /**
   * @brief Reads pagination query parameters from an HTTP request.
   *
   * Supported query parameters:
   *
   * - limit
   * - offset
   *
   * Values are clamped to safe bounds.
   *
   * @param req HTTP request.
   * @param default_limit Default limit when missing or invalid.
   * @param max_limit Maximum allowed limit.
   * @return Parsed pagination values.
   */
  [[nodiscard]] Pagination read_pagination(
      vix::Request &req,
      std::size_t default_limit = 25,
      std::size_t max_limit = 100);
}

#endif // PICO_SUPPORT_HTTP_QUERY_HPP
