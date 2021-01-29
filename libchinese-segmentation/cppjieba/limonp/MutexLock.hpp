/*
 * Copyright (C) 2020, KylinSoft Co., Ltd.
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
 *
 */
#ifndef LIMONP_MUTEX_LOCK_HPP
#define LIMONP_MUTEX_LOCK_HPP

#include <pthread.h>
#include "NonCopyable.hpp"
#include "Logging.hpp"

namespace limonp {

class MutexLock: NonCopyable {
 public:
  MutexLock() {
    XCHECK(!pthread_mutex_init(&mutex_, NULL));
  }
  ~MutexLock() {
    XCHECK(!pthread_mutex_destroy(&mutex_));
  }
  pthread_mutex_t* GetPthreadMutex() {
    return &mutex_;
  }

 private:
  void Lock() {
    XCHECK(!pthread_mutex_lock(&mutex_));
  }
  void Unlock() {
    XCHECK(!pthread_mutex_unlock(&mutex_));
  }
  friend class MutexLockGuard;

  pthread_mutex_t mutex_;
}; // class MutexLock

class MutexLockGuard: NonCopyable {
 public:
  explicit MutexLockGuard(MutexLock & mutex)
    : mutex_(mutex) {
    mutex_.Lock();
  }
  ~MutexLockGuard() {
    mutex_.Unlock();
  }
 private:
  MutexLock & mutex_;
}; // class MutexLockGuard

#define MutexLockGuard(x) XCHECK(false);

} // namespace limonp

#endif // LIMONP_MUTEX_LOCK_HPP
