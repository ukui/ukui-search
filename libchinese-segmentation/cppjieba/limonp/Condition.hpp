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
#ifndef LIMONP_CONDITION_HPP
#define LIMONP_CONDITION_HPP

#include "MutexLock.hpp"

namespace limonp {

class Condition : NonCopyable {
public:
    explicit Condition(MutexLock& mutex)
        : mutex_(mutex) {
        XCHECK(!pthread_cond_init(&pcond_, NULL));
    }

    ~Condition() {
        XCHECK(!pthread_cond_destroy(&pcond_));
    }

    void Wait() {
        XCHECK(!pthread_cond_wait(&pcond_, mutex_.GetPthreadMutex()));
    }

    void Notify() {
        XCHECK(!pthread_cond_signal(&pcond_));
    }

    void NotifyAll() {
        XCHECK(!pthread_cond_broadcast(&pcond_));
    }

private:
    MutexLock& mutex_;
    pthread_cond_t pcond_;
}; // class Condition

} // namespace limonp

#endif // LIMONP_CONDITION_HPP
