#ifndef LOCKQUEUE_H
#define LOCKQUEUE_H
#include <queue>
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>

// 异步写日志队列
template<typename T>
class LockQueue {
public:
  // 工作线程写日志 queue
  void Push(const T &data) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(data);
    condVariable_.notify_one();
  }

  // 单线程读日志 queue，将其写入磁盘
  T Pop() {
    std::unique_lock<std::mutex> lock(mutex_);
    // 如果队列为空，线程进入 wait 状态
    while (queue_.empty()) {
      condVariable_.wait(lock);
    }
    T data = queue_.front();
    queue_.pop();
    return data;
  }

private:
  std::queue<T> queue_;
  std::mutex mutex_;
  std::condition_variable condVariable_;
};





#endif