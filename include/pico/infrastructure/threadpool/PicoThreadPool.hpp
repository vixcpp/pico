/**
 *
 *  @file PicoThreadPool.hpp
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

#ifndef PICO_INFRASTRUCTURE_THREADPOOL_PICO_THREAD_POOL_HPP
#define PICO_INFRASTRUCTURE_THREADPOOL_PICO_THREAD_POOL_HPP

#include <cstddef>
#include <cstdint>
#include <string>

#include <vix/threadpool.hpp>

namespace pico::infrastructure::threadpool
{
  /**
   * @brief Owns Pico's background worker pool.
   *
   * PicoThreadPool centralizes background execution so controllers and
   * services can submit small diagnostic jobs without managing worker
   * lifecycle directly.
   */
  class PicoThreadPool
  {
  public:
    /**
     * @brief Creates a thread pool using Pico defaults.
     */
    PicoThreadPool();

    /**
     * @brief Creates a thread pool with a fixed worker count.
     *
     * @param thread_count Number of worker threads.
     */
    explicit PicoThreadPool(std::size_t thread_count);

    PicoThreadPool(const PicoThreadPool &) = delete;
    PicoThreadPool &operator=(const PicoThreadPool &) = delete;

    PicoThreadPool(PicoThreadPool &&) noexcept = delete;
    PicoThreadPool &operator=(PicoThreadPool &&) noexcept = delete;

    ~PicoThreadPool();

    /**
     * @brief Returns true when the underlying pool is running.
     */
    [[nodiscard]] bool running() const noexcept;

    /**
     * @brief Waits until accepted work is completed.
     */
    void wait_idle();

    /**
     * @brief Shuts down the pool.
     */
    void shutdown() noexcept;

    /**
     * @brief Submits a simple background job and waits for the result.
     *
     * @param value Input value.
     * @return Computed result.
     */
    [[nodiscard]] int run_job(int value);

    /**
     * @brief Runs a deliberately slow job with an observational timeout.
     *
     * @return Human-readable task result status.
     */
    [[nodiscard]] std::string run_slow_job();

    /**
     * @brief Returns current pool metrics.
     */
    [[nodiscard]] vix::threadpool::ThreadPoolMetrics metrics() const;

    /**
     * @brief Returns cumulative pool stats.
     */
    [[nodiscard]] vix::threadpool::ThreadPoolStats stats() const;

    /**
     * @brief Returns the underlying Vix thread pool.
     */
    [[nodiscard]] vix::threadpool::ThreadPool &pool();

    /**
     * @brief Returns the underlying Vix thread pool.
     */
    [[nodiscard]] const vix::threadpool::ThreadPool &pool() const;

  private:
    vix::threadpool::ThreadPool pool_;
  };
}

#endif // PICO_INFRASTRUCTURE_THREADPOOL_PICO_THREAD_POOL_HPP
