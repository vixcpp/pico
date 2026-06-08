/**
 *
 *  @file PicoKvStore.cpp
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

#include <pico/infrastructure/kv/PicoKvStore.hpp>

#include <cstdlib>
#include <stdexcept>
#include <utility>

namespace pico::infrastructure::kv
{
  namespace
  {
    constexpr const char *boot_count_key = "app/boot_count";
    constexpr const char *last_heartbeat_key = "app/last_heartbeat";

    [[nodiscard]] std::int64_t parse_counter(
        const std::optional<std::string> &value) noexcept
    {
      if (!value.has_value())
      {
        return 0;
      }

      try
      {
        return std::stoll(*value);
      }
      catch (...)
      {
        return 0;
      }
    }
  }

  PicoKvStore::PicoKvStore()
      : PicoKvStore("storage/kv")
  {
  }

  PicoKvStore::PicoKvStore(std::string path)
      : path_(std::move(path)),
        store_(std::nullopt)
  {
  }

  void PicoKvStore::open()
  {
    if (store_.has_value())
    {
      return;
    }

    store_.emplace(vix::kv::open(path_));
  }

  void PicoKvStore::close()
  {
    if (!store_.has_value())
    {
      return;
    }

    (void)store_->flush();
    (void)store_->close();

    store_.reset();
  }

  bool PicoKvStore::opened() const noexcept
  {
    return store_.has_value();
  }

  const std::string &PicoKvStore::path() const noexcept
  {
    return path_;
  }

  void PicoKvStore::put(const std::string &key, const std::string &value)
  {
    store().put(key, value);
    (void)store().flush();
  }

  std::optional<std::string> PicoKvStore::get(const std::string &key) const
  {
    return store().get(key);
  }

  void PicoKvStore::erase(const std::string &key)
  {
    const auto erased = store().erase(vix::kv::KeyPath{key});

    if (erased.is_err())
    {
      throw std::runtime_error{erased.error().message()};
    }

    (void)store().flush();
  }

  std::int64_t PicoKvStore::increment(const std::string &key)
  {
    const auto next = counter(key) + 1;

    put(key, std::to_string(next));

    return next;
  }

  std::int64_t PicoKvStore::counter(const std::string &key) const
  {
    return parse_counter(get(key));
  }

  std::int64_t PicoKvStore::increment_boot_count()
  {
    return increment(boot_count_key);
  }

  void PicoKvStore::set_last_heartbeat(const std::string &value)
  {
    put(last_heartbeat_key, value);
  }

  std::optional<std::string> PicoKvStore::last_heartbeat() const
  {
    return get(last_heartbeat_key);
  }

  vix::kv::Kv &PicoKvStore::store()
  {
    if (!store_.has_value())
    {
      throw std::runtime_error{"Pico KV store is not open"};
    }

    return *store_;
  }

  const vix::kv::Kv &PicoKvStore::store() const
  {
    if (!store_.has_value())
    {
      throw std::runtime_error{"Pico KV store is not open"};
    }

    return *store_;
  }
}
