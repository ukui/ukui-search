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
#ifndef LIMONP_THREAD_HPP
#define LIMONP_THREAD_HPP

#include "Logging.hpp"
#include "NonCopyable.hpp"

namespace limonp {

class IThread: NonCopyable {
public:
    IThread(): isStarted(false), isJoined(false) {
    }
    virtual ~IThread() {
        if(isStarted && !isJoined) {
            XCHECK(!pthread_detach(thread_));
        }
    };

    virtual void Run() = 0;
    void Start() {
        XCHECK(!isStarted);
        XCHECK(!pthread_create(&thread_, NULL, Worker, this));
        isStarted = true;
    }
    void Join() {
        XCHECK(!isJoined);
        XCHECK(!pthread_join(thread_, NULL));
        isJoined = true;
    }
private:
    static void * Worker(void * data) {
        IThread * ptr = (IThread*) data;
        ptr->Run();
        return NULL;
    }

    pthread_t thread_;
    bool isStarted;
    bool isJoined;
}; // class IThread

} // namespace limonp

#endif // LIMONP_THREAD_HPP
