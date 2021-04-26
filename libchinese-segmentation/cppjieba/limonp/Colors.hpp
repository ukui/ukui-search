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
#ifndef LIMONP_COLOR_PRINT_HPP
#define LIMONP_COLOR_PRINT_HPP

#include <string>
#include <stdarg.h>

namespace limonp {

using std::string;

enum Color {
    BLACK = 30,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    PURPLE
}; // enum Color

static void ColorPrintln(enum Color color, const char * fmt, ...) {
    va_list ap;
    printf("\033[0;%dm", color);
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\033[0m\n"); // if not \n , in some situation , the next lines will be set the same color unexpectedly
}

} // namespace limonp

#endif // LIMONP_COLOR_PRINT_HPP
