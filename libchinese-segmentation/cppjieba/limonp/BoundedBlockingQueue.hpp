/*
 * Copyright (C) 2022, KylinSoft Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: yanyiwu
 * Modified by: jixiaoxu <jixiaoxu@kylinos.cn>
 *
 */
#ifndef LIMONP_BOUNDED_BLOCKING_QUEUE_HPP
#define LIMONP_BOUNDED_BLOCKING_QUEUE_HPP

#include "BoundedQueue.hpp"

namespace limonp {

template<typename T>
class BoundedBlockingQueue : NonCopyable {
 public:
  explicit BoundedBlockingQueue(size_t maxSize)
    : mutex_(),
      notEmpty_(mutex_),
      notFull_(mutex_),
      queue_(maxSize) {
  }

  void Push(const T& x) {
    MutexLockGuard lock(mutex_);
    while (queue_.Full()) {
      notFull_.Wait();
    }
    assert(!queue_.Full());
    queue_.Push(x);
    notEmpty_.Notify();
  }

  T Pop() {
    MutexLockGuard lock(mutex_);
    while (queue_.Empty()) {
      notEmpty_.Wait();
    }
    assert(!queue_.Empty());
    T res = queue_.Pop();
    notFull_.Notify();
    return res;
  }

  bool Empty() const {
    MutexLockGuard lock(mutex_);
    return queue_.Empty();
  }

  bool Full() const {
    MutexLockGuard lock(mutex_);
    return queue_.Full();
  }

  size_t size() const {
    MutexLockGuard lock(mutex_);
    return queue_.size();
  }

  size_t capacity() const {
    return queue_.capacity();
  }

 private:
  mutable MutexLock          mutex_;
  Condition                  notEmpty_;
  Condition                  notFull_;
  BoundedQueue<T>  queue_;
}; // class BoundedBlockingQueue

} // namespace limonp

#endif // LIMONP_BOUNDED_BLOCKING_QUEUE_HPP
