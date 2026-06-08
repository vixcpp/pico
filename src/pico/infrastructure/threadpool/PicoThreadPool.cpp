/**
 *
 *  @file PicoThreadPool.cpp
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

#include <pico/infrastructure/threadpool/PicoThreadPool.hpp>

#include <chrono>
#include <thread>

namespace pico::infrastructure::threadpool
{
  namespace
  {
    [[nodiscard]] vix::threadpool::ThreadPoolConfig make_config(
        std::size_t thread_count)
    {
      vix::threadpool::ThreadPoolConfig config;

      config.thread_count = thread_count;
      config.max_thread_count = thread_count;
      config.max_queue_size = 1024;
      config.drain_on_shutdown = true;

      return config;
    }
  }

  PicoThreadPool::PicoThreadPool()
      : PicoThreadPool(4)
  {
  }

  PicoThreadPool::PicoThreadPool(std::size_t thread_count)
      : pool_(make_config(thread_count == 0 ? 1 : thread_count))
  {
  }

  PicoThreadPool::~PicoThreadPool()
  {
    shutdown();
  }

  bool PicoThreadPool::running() const noexcept
  {
    return pool_.running();
  }

  void PicoThreadPool::wait_idle()
  {
    pool_.wait_idle();
  }

  void PicoThreadPool::shutdown() noexcept
  {
    pool_.shutdown();
  }

  int PicoThreadPool::run_job(int value)
  {
    auto future = pool_.submit([value]()
                               { return value * 2; });

    return future.get();
  }

  std::string PicoThreadPool::run_slow_job()
  {
    vix::threadpool::TaskOptions options;

    options.set_timeout(vix::threadpool::Timeout::milliseconds(50));

    auto future = pool_.submit(
        []()
        {
          std::this_thread::sleep_for(std::chrono::milliseconds{100});
          return 42;
        },
        options);

    try
    {
      (void)future.get();
    }
    catch (...)
    {
    }

    return vix::threadpool::to_string(future.result());
  }

  vix::threadpool::ThreadPoolMetrics PicoThreadPool::metrics() const
  {
    return pool_.metrics();
  }

  vix::threadpool::ThreadPoolStats PicoThreadPool::stats() const
  {
    return pool_.stats();
  }

  vix::threadpool::ThreadPool &PicoThreadPool::pool()
  {
    return pool_;
  }

  const vix::threadpool::ThreadPool &PicoThreadPool::pool() const
  {
    return pool_;
  }
}
