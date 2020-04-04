// Copyright 2004-present Facebook. All Rights Reserved.

#include "util/async.h"

#include <future>
#include <iostream>
#include <mutex>
#include <queue>

namespace tinyrt {
namespace {
class ThreadPool final {
 public:
  explicit ThreadPool(unsigned num_threads) {
    std::cout << "Thread pool created with " << num_threads << " threads."
              << std::endl;
    for (auto i = 0U; i < num_threads; ++i) {
      workers_.push_back(std::async(std::launch::async, [this] {
        while (true) {
          std::function<void()> func;
          {
            std::unique_lock<std::mutex> l(mu_);
            cv_.wait(l, [this] { return !queue_.empty(); });
            func = std::move(queue_.front());
            queue_.pop();
          }
          if (func == nullptr) {  // Shutdown signal.
            break;
          }
          func();
        }
      }));
    }
  }

  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

  ~ThreadPool() {
    {
      std::lock_guard<std::mutex> l(mu_);
      for (auto i = 0U; i < workers_.size(); i++) {
        queue_.push(nullptr);  // Shutdown signal.
      }
    }
    cv_.notify_all();
    for (auto& worker : workers_) {
      worker.wait();
    }
  }

  void submit(const std::function<void()>& function) {
    {
      std::lock_guard<std::mutex> l(mu_);
      queue_.push(function);
    }
    cv_.notify_one();
  }

 private:
  std::mutex mu_;
  std::condition_variable cv_;
  std::queue<std::function<void()>> queue_;
  std::vector<std::future<void>> workers_;
};
};  // namespace

void Async::submit(const std::function<void()>& function) {
  static ThreadPool threadPool(std::thread::hardware_concurrency());
  threadPool.submit(function);
}
}  // namespace tinyrt
