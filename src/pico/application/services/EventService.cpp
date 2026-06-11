/**
 *
 *  @file EventService.cpp
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

#include <pico/application/services/EventService.hpp>

#include <stdexcept>

namespace pico::application::services
{
  EventService::EventService(
      infrastructure::database::EventRepository &repository)
      : repository_(&repository)
  {
  }

  void EventService::initialize()
  {
    repository_->ensure_schema();
  }

  std::int64_t EventService::record(
      const std::string &source,
      const std::string &type,
      const std::string &payload)
  {
    if (source.empty())
    {
      throw std::invalid_argument{"event source is required"};
    }

    if (type.empty())
    {
      throw std::invalid_argument{"event type is required"};
    }

    return repository_->create(source, type, payload);
  }

  std::vector<domain::models::Event> EventService::latest(
      std::size_t limit) const
  {
    return repository_->latest(limit);
  }

  std::vector<domain::models::Event> EventService::latest(
      std::size_t limit,
      std::size_t offset) const
  {
    return repository_->latest(limit, offset);
  }

  std::int64_t EventService::count() const
  {
    return repository_->count();
  }
}
