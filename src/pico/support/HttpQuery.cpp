/**
 *
 *  @file HttpQuery.cpp
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

#include <pico/support/HttpQuery.hpp>

#include <algorithm>
#include <string>

namespace pico::support
{
  namespace
  {
    [[nodiscard]] std::size_t parse_size_or(
        const std::string &value,
        std::size_t fallback) noexcept
    {
      if (value.empty())
      {
        return fallback;
      }

      try
      {
        const auto parsed = std::stoull(value);
        return static_cast<std::size_t>(parsed);
      }
      catch (...)
      {
        return fallback;
      }
    }
  }

  Pagination read_pagination(
      vix::Request &req,
      std::size_t default_limit,
      std::size_t max_limit)
  {
    const auto safe_max = max_limit == 0 ? std::size_t{100} : max_limit;

    const auto safe_default = std::clamp<std::size_t>(
        default_limit,
        1,
        safe_max);

    const auto raw_limit = req.query_value("limit", "");
    const auto raw_offset = req.query_value("offset", "");

    Pagination pagination{};

    pagination.limit = std::clamp<std::size_t>(
        parse_size_or(raw_limit, safe_default),
        1,
        safe_max);

    pagination.offset = parse_size_or(raw_offset, 0);

    return pagination;
  }
}
