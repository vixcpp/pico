/**
 *
 *  @file DatabaseProvider.cpp
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

#include <pico/infrastructure/database/DatabaseProvider.hpp>

#include <stdexcept>
#include <utility>

namespace pico::infrastructure::database
{
  DatabaseProvider::DatabaseProvider()
      : DatabaseProvider("storage/pico.db")
  {
  }

  DatabaseProvider::DatabaseProvider(std::string path)
      : path_(std::move(path)),
        database_(nullptr)
  {
  }

  void DatabaseProvider::open()
  {
    if (database_)
    {
      return;
    }

    database_ = std::make_unique<vix::db::Database>(
        vix::db::Database::sqlite(path_));
  }

  vix::db::Database &DatabaseProvider::database()
  {
    if (!database_)
    {
      throw std::runtime_error{"Pico database is not open"};
    }

    return *database_;
  }

  const vix::db::Database &DatabaseProvider::database() const
  {
    if (!database_)
    {
      throw std::runtime_error{"Pico database is not open"};
    }

    return *database_;
  }

  bool DatabaseProvider::opened() const noexcept
  {
    return database_ != nullptr;
  }

  const std::string &DatabaseProvider::path() const noexcept
  {
    return path_;
  }
}
