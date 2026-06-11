/**
 *
 *  @file EventRepository.cpp
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

#include <pico/infrastructure/database/EventRepository.hpp>

#include <algorithm>
#include <stdexcept>

namespace pico::infrastructure::database
{
  EventRepository::EventRepository(DatabaseProvider &provider)
      : provider_(&provider)
  {
  }

  void EventRepository::ensure_schema()
  {
    provider_->database().exec(
        "CREATE TABLE IF NOT EXISTS events ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "source TEXT NOT NULL,"
        "type TEXT NOT NULL,"
        "payload TEXT NOT NULL,"
        "created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP"
        ")");
  }

  std::int64_t EventRepository::create(
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

    provider_->database().exec(
        "INSERT INTO events (source, type, payload) VALUES (?, ?, ?)",
        source,
        type,
        payload);

    auto rows = provider_->database().query(
        "SELECT last_insert_rowid()");

    if (rows->next())
    {
      return rows->row().getInt64(0);
    }

    return 0;
  }

  std::vector<domain::models::Event> EventRepository::latest(
      std::size_t limit) const
  {
    return latest(limit, 0);
  }

  std::vector<domain::models::Event> EventRepository::latest(
      std::size_t limit,
      std::size_t offset) const
  {
    const auto safe_limit = std::clamp<std::size_t>(limit, 1, 100);

    auto rows = provider_->database().query(
        "SELECT id, source, type, payload, created_at "
        "FROM events "
        "ORDER BY id DESC "
        "LIMIT ? OFFSET ?",
        static_cast<std::int64_t>(safe_limit),
        static_cast<std::int64_t>(offset));

    std::vector<domain::models::Event> events{};

    while (rows->next())
    {
      const auto &row = rows->row();

      domain::models::Event event{};
      event.id = row.getInt64(0);
      event.source = row.getString(1);
      event.type = row.getString(2);
      event.payload = row.getString(3);
      event.created_at = row.getString(4);

      events.push_back(std::move(event));
    }

    return events;
  }

  std::int64_t EventRepository::count() const
  {
    auto rows = provider_->database().query(
        "SELECT COUNT(*) FROM events");

    if (rows->next())
    {
      return rows->row().getInt64(0);
    }

    return 0;
  }
}
