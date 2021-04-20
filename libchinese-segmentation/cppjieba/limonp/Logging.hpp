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
#ifndef LIMONP_LOGGING_HPP
#define LIMONP_LOGGING_HPP

#include <sstream>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <ctime>

#ifdef XLOG
#error "XLOG has been defined already"
#endif // XLOG
#ifdef XCHECK
#error "XCHECK has been defined already"
#endif // XCHECK

#define XLOG(level) limonp::Logger(limonp::LL_##level, __FILE__, __LINE__).Stream()
#define XCHECK(exp) if(!(exp)) XLOG(FATAL) << "exp: ["#exp << "] false. "

namespace limonp {

enum {
    LL_DEBUG = 0,
    LL_INFO = 1,
    LL_WARNING = 2,
    LL_ERROR = 3,
    LL_FATAL = 4,
}; // enum

static const char * LOG_LEVEL_ARRAY[] = {"DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
static const char * LOG_TIME_FORMAT = "%Y-%m-%d %H:%M:%S";

class Logger {
public:
    Logger(size_t level, const char* filename, int lineno)
        : level_(level) {
#ifdef LOGGING_LEVEL
        if(level_ < LOGGING_LEVEL) {
            return;
        }
#endif
        assert(level_ <= sizeof(LOG_LEVEL_ARRAY) / sizeof(*LOG_LEVEL_ARRAY));
        char buf[32];
        time_t now;
        time(&now);
        strftime(buf, sizeof(buf), LOG_TIME_FORMAT, localtime(&now));
        stream_ << buf
                << " " << filename
                << ":" << lineno
                << " " << LOG_LEVEL_ARRAY[level_]
                << " ";
    }
    ~Logger() {
#ifdef LOGGING_LEVEL
        if(level_ < LOGGING_LEVEL) {
            return;
        }
#endif
        std::cerr << stream_.str() << std::endl;
        if(level_ == LL_FATAL) {
            abort();
        }
    }

    std::ostream& Stream() {
        return stream_;
    }

private:
    std::ostringstream stream_;
    size_t level_;
}; // class Logger

} // namespace limonp

#endif // LIMONP_LOGGING_HPP
