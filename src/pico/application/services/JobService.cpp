/**
 *
 *  @file JobService.cpp
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

#include <pico/application/services/JobService.hpp>

#include <stdexcept>

namespace pico::application::services
{
  JobService::JobService(
      infrastructure::threadpool::PicoThreadPool &pool,
      infrastructure::kv::PicoKvStore &kv,
      EventService &events)
      : pool_(&pool),
        kv_(&kv),
        events_(&events)
  {
  }

  vix::json::Json JobService::run_job(
      const std::string &name,
      int value)
  {
    if (name.empty())
    {
      throw std::invalid_argument{"job name is required"};
    }

    const auto result = pool_->run_job(value);
    const auto run_count = kv_->increment("jobs/run_count");

    using namespace vix::json;

    const auto payload = o(
        "name", name,
        "value", value,
        "result", result,
        "run_count", run_count);

    (void)events_->record(
        "job",
        "job.completed",
        dumps_compact(payload));

    return o(
        "ok", true,
        "job", name,
        "value", value,
        "result", result,
        "run_count", run_count);
  }

  vix::json::Json JobService::run_slow_job()
  {
    const auto result = pool_->run_slow_job();
    const auto run_count = kv_->increment("jobs/slow_run_count");

    using namespace vix::json;

    const auto payload = o(
        "name", "slow",
        "result", result,
        "run_count", run_count);

    (void)events_->record(
        "job",
        "job.slow.completed",
        dumps_compact(payload));

    return o(
        "ok", true,
        "job", "slow",
        "result", result,
        "run_count", run_count);
  }
}
