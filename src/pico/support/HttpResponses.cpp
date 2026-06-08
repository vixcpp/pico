/**
 *
 *  @file HttpResponses.cpp
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

#include <pico/support/HttpResponses.hpp>
#include <vix/json.hpp>

namespace pico::support
{
  void json_error(
      vix::Response &res,
      int status,
      const std::string &code,
      const std::string &message)
  {
    using namespace vix::json;

    res.status(status).json(o(
        "ok", false,
        "error", code,
        "message", message));
  }

  void json_ok(
      vix::Response &res,
      const vix::json::Json &data)
  {
    using namespace vix::json;

    res.json(o(
        "ok", true,
        "data", data));
  }

  void json_message(
      vix::Response &res,
      const std::string &message)
  {
    using namespace vix::json;

    res.json(o(
        "ok", true,
        "message", message));
  }
}
