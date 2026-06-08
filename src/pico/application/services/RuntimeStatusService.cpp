/**
 *
 *  @file RuntimeStatusService.cpp
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

#include <pico/application/services/RuntimeStatusService.hpp>

namespace pico::application::services
{
  RuntimeStatusService::RuntimeStatusService(
      infrastructure::kv::PicoKvStore &kv,
      infrastructure::threadpool::PicoThreadPool &pool,
      EventService &events)
      : kv_(&kv),
        pool_(&pool),
        events_(&events),
        started_at_(std::chrono::steady_clock::now())
  {
  }

  std::int64_t RuntimeStatusService::mark_boot()
  {
    const auto boot_count = kv_->increment_boot_count();

    using namespace vix::json;

    const auto payload = o(
        "boot_count", boot_count);

    (void)events_->record(
        "system",
        "app.boot",
        dumps_compact(payload));

    return boot_count;
  }

  void RuntimeStatusService::heartbeat(const std::string &value)
  {
    kv_->set_last_heartbeat(value);

    using namespace vix::json;

    const auto payload = o(
        "last_heartbeat", value);

    (void)events_->record(
        "system",
        "app.heartbeat",
        dumps_compact(payload));
  }

  std::int64_t RuntimeStatusService::increment_http_requests()
  {
    return kv_->increment("http/requests");
  }

  std::int64_t RuntimeStatusService::increment_ws_messages()
  {
    return kv_->increment("ws/messages");
  }

  std::int64_t RuntimeStatusService::uptime_seconds() const
  {
    const auto now = std::chrono::steady_clock::now();
    const auto elapsed = now - started_at_;

    return std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
  }

  vix::json::Json RuntimeStatusService::status() const
  {
    using namespace vix::json;

    return o(
        "ok", true,
        "app", "pico",
        "runtime", "vix.cpp",
        "status", "running",
        "uptime_seconds", uptime_seconds(),
        "boot_count", kv_->counter("app/boot_count"),
        "last_heartbeat", kv_->last_heartbeat().value_or(""),
        "http_requests", kv_->counter("http/requests"),
        "ws_messages", kv_->counter("ws/messages"),
        "job_runs", kv_->counter("jobs/run_count"),
        "slow_job_runs", kv_->counter("jobs/slow_run_count"),
        "events", o("total", events_->count()),
        "threadpool", threadpool_status());
  }

  vix::json::Json RuntimeStatusService::health() const
  {
    using namespace vix::json;

    return o(
        "ok", true,
        "status", "ok",
        "app", "pico",
        "runtime", "vix.cpp",
        "uptime_seconds", uptime_seconds());
  }

  vix::json::Json RuntimeStatusService::threadpool_status() const
  {
    const auto metrics = pool_->metrics();
    const auto stats = pool_->stats();

    using namespace vix::json;

    return o(
        "running", pool_->running(),
        "metrics", o("workers", metrics.worker_count, "pending", metrics.pending_tasks, "active", metrics.active_tasks, "idle_workers", metrics.idle_workers, "busy_workers", metrics.busy_workers, "submitted", metrics.submitted_tasks, "completed", metrics.completed_tasks, "failed", metrics.failed_tasks, "cancelled", metrics.cancelled_tasks, "timed_out", metrics.timed_out_tasks, "rejected", metrics.rejected_tasks),
        "stats", o("accepted", stats.accepted_tasks, "rejected", stats.rejected_tasks, "completed", stats.completed_tasks, "failed", stats.failed_tasks, "cancelled", stats.cancelled_tasks, "timed_out", stats.timed_out_tasks));
  }
}
