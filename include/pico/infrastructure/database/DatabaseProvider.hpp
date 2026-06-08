/**
 *
 *  @file DatabaseProvider.hpp
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

#ifndef PICO_INFRASTRUCTURE_DATABASE_DATABASE_PROVIDER_HPP
#define PICO_INFRASTRUCTURE_DATABASE_DATABASE_PROVIDER_HPP

#include <memory>
#include <string>

#include <vix/db/Database.hpp>

namespace pico::infrastructure::database
{
  /**
   * @brief Owns and exposes Pico's SQLite database connection.
   *
   * DatabaseProvider keeps database creation in one infrastructure component
   * instead of spreading database opening logic across controllers and services.
   */
  class DatabaseProvider
  {
  public:
    /**
     * @brief Creates a provider using the default Pico SQLite path.
     */
    DatabaseProvider();

    /**
     * @brief Creates a provider using a custom SQLite database path.
     *
     * @param path SQLite database file path.
     */
    explicit DatabaseProvider(std::string path);

    DatabaseProvider(const DatabaseProvider &) = delete;
    DatabaseProvider &operator=(const DatabaseProvider &) = delete;

    DatabaseProvider(DatabaseProvider &&) noexcept = default;
    DatabaseProvider &operator=(DatabaseProvider &&) noexcept = default;

    ~DatabaseProvider() = default;

    /**
     * @brief Opens the database if it is not already open.
     */
    void open();

    /**
     * @brief Returns the active database connection.
     *
     * @return Reference to the Vix database facade.
     */
    [[nodiscard]] vix::db::Database &database();

    /**
     * @brief Returns the active database connection.
     *
     * @return Const reference to the Vix database facade.
     */
    [[nodiscard]] const vix::db::Database &database() const;

    /**
     * @brief Returns true when the provider has opened the database.
     */
    [[nodiscard]] bool opened() const noexcept;

    /**
     * @brief Returns the configured SQLite database path.
     */
    [[nodiscard]] const std::string &path() const noexcept;

  private:
    std::string path_;
    std::unique_ptr<vix::db::Database> database_;
  };
}

#endif // PICO_INFRASTRUCTURE_DATABASE_DATABASE_PROVIDER_HPP
