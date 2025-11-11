#pragma once
#include <atomic>
#include <thread>

class SpinLock {
  public:
  void lock() {
    while (flag.test_and_set(std::memory_order_acquire)) {
      std::this_thread::yield();
    }
  }
  void unlock() { flag.clear(std::memory_order_release); }
  bool try_lock() { return !flag.test_and_set(std::memory_order_acquire); }

  private:
  std::atomic_flag flag = ATOMIC_FLAG_INIT;
};

class SpinLockGuard {
  public:
  /**
   * @brief SpinLockのRAII型スコープロック
   * @code
   * SpinLock lock;
   * {
   *     SpinLockGuard guard(lock); // このスコープでlockが取得される
   *     // クリティカルセクション
   * }
   * // スコープを抜けると自動でunlock
   * @endcode
   */
  explicit SpinLockGuard(SpinLock& lock) : lock_(lock) { lock_.lock(); }
  ~SpinLockGuard() { lock_.unlock(); }
  SpinLockGuard(const SpinLockGuard&) = delete;
  SpinLockGuard& operator=(const SpinLockGuard&) = delete;
  SpinLockGuard(SpinLockGuard&&) = default;
  SpinLockGuard& operator=(SpinLockGuard&&) = default;

  private:
  SpinLock& lock_;
};
