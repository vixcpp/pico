/**
 *
 *  @file JsonHelpers.cpp
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

#include <pico/support/JsonHelpers.hpp>

namespace pico::support
{
  vix::json::Json make_error_json(
      const std::string &code,
      const std::string &message)
  {
    using namespace vix::json;

    return o(
        "ok", false,
        "error", o("code", code, "message", message));
  }

  vix::json::Json make_ok_json(const std::string &message)
  {
    using namespace vix::json;

    return o(
        "ok", true,
        "message", message);
  }

  vix::json::Json event_to_json(const domain::models::Event &event)
  {
    using namespace vix::json;

    return o(
        "id", event.id,
        "source", event.source,
        "type", event.type,
        "payload", event.payload,
        "created_at", event.created_at);
  }

  vix::json::Json events_to_json(
      const std::vector<domain::models::Event> &events)
  {
    using namespace vix::json;

    Json output = Json::array();

    for (const auto &event : events)
    {
      output.push_back(event_to_json(event));
    }

    return output;
  }
}
