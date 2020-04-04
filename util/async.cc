// Copyright 2004-present Facebook. All Rights Reserved.

#include "util/async.h"

#include <mutex>
#include <queue>
#include <thread>

namespace tinyrt {
class ThreadPool final {
 public:
  explicit ThreadPool(int num_threads) {
    for (int i = 0; i < num_threads; ++i) {
      threads_.push_back(std::thread(&ThreadPool::WorkLoop, this));
    }
  }

  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

  ~ThreadPool() {
    {
      std::lock_guard<std::mutex> l(mu_);
      for (size_t i = 0; i < threads_.size(); i++) {
        queue_.push(nullptr);  // Shutdown signal.
      }
    }
    for (auto& t : threads_) {
      t.join();
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
  void WorkLoop() {
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
  }

  std::mutex mu_;
  std::condition_variable cv_;
  std::queue<std::function<void()>> queue_;
  std::vector<std::thread> threads_;
};

Async& Async::instance() {
  static Async async;  // singleton.
  return async;
}

Async::Async()
    : threadPool_(
          std::make_unique<ThreadPool>(std::thread::hardware_concurrency())) {}

Async::~Async() = default;

void Async::submit(const std::function<void()>& function) {
  threadPool_->submit(function);
}
}  // namespace tinyrt
