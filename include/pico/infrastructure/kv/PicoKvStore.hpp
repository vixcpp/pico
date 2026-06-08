/**
 *
 *  @file PicoKvStore.hpp
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

#ifndef PICO_INFRASTRUCTURE_KV_PICO_KV_STORE_HPP
#define PICO_INFRASTRUCTURE_KV_PICO_KV_STORE_HPP

#include <cstdint>
#include <optional>
#include <string>

#include <vix/kv.hpp>

namespace pico::infrastructure::kv
{
  /**
   * @brief Small durable KV wrapper used by Pico runtime diagnostics.
   *
   * PicoKvStore stores lightweight runtime state such as boot count,
   * heartbeat time, HTTP counters, WebSocket counters, and background job
   * counters. The data is persisted through Vix KV so restart recovery can
   * be tested in production.
   */
  class PicoKvStore
  {
  public:
    /**
     * @brief Creates a KV store using the default Pico path.
     */
    PicoKvStore();

    /**
     * @brief Creates a KV store using a custom durable path.
     *
     * @param path Durable KV directory path.
     */
    explicit PicoKvStore(std::string path);

    PicoKvStore(const PicoKvStore &) = delete;
    PicoKvStore &operator=(const PicoKvStore &) = delete;

    PicoKvStore(PicoKvStore &&) noexcept = default;
    PicoKvStore &operator=(PicoKvStore &&) noexcept = default;

    ~PicoKvStore() = default;

    /**
     * @brief Opens the durable KV store if it is not already open.
     */
    void open();

    /**
     * @brief Flushes and closes the KV store.
     */
    void close();

    /**
     * @brief Returns true when the KV store is open.
     */
    [[nodiscard]] bool opened() const noexcept;

    /**
     * @brief Returns the configured durable KV path.
     */
    [[nodiscard]] const std::string &path() const noexcept;

    /**
     * @brief Writes a string value.
     *
     * @param key Flat key.
     * @param value String value.
     */
    void put(const std::string &key, const std::string &value);

    /**
     * @brief Reads a string value.
     *
     * @param key Flat key.
     * @return Stored value when present.
     */
    [[nodiscard]] std::optional<std::string> get(
        const std::string &key) const;

    /**
     * @brief Deletes a flat key.
     *
     * @param key Flat key.
     */
    void erase(const std::string &key);

    /**
     * @brief Increments an integer counter stored as a string.
     *
     * @param key Counter key.
     * @return New counter value.
     */
    [[nodiscard]] std::int64_t increment(const std::string &key);

    /**
     * @brief Reads an integer counter stored as a string.
     *
     * @param key Counter key.
     * @return Counter value, or zero when missing or invalid.
     */
    [[nodiscard]] std::int64_t counter(const std::string &key) const;

    /**
     * @brief Increments and returns the boot counter.
     *
     * @return New boot counter value.
     */
    [[nodiscard]] std::int64_t increment_boot_count();

    /**
     * @brief Stores the latest heartbeat value.
     *
     * @param value Heartbeat value, usually an ISO date or unix timestamp.
     */
    void set_last_heartbeat(const std::string &value);

    /**
     * @brief Returns the latest heartbeat value.
     *
     * @return Stored heartbeat when present.
     */
    [[nodiscard]] std::optional<std::string> last_heartbeat() const;

    /**
     * @brief Returns the underlying Vix KV database.
     */
    [[nodiscard]] vix::kv::Kv &store();

    /**
     * @brief Returns the underlying Vix KV database.
     */
    [[nodiscard]] const vix::kv::Kv &store() const;

  private:
    std::string path_;
    std::optional<vix::kv::Kv> store_;
  };
}

#endif // PICO_INFRASTRUCTURE_KV_PICO_KV_STORE_HPP
