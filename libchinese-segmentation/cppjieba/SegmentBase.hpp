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
#ifndef CPPJIEBA_SEGMENTBASE_H
#define CPPJIEBA_SEGMENTBASE_H

#include "limonp/Logging.hpp"
#include "PreFilter.hpp"
#include <cassert>


namespace cppjieba {

const char* const SPECIAL_SEPARATORS = " \t\n\xEF\xBC\x8C\xE3\x80\x82";

using namespace limonp;

class SegmentBase {
 public:
  SegmentBase() {
    XCHECK(ResetSeparators(SPECIAL_SEPARATORS));
  }
  virtual ~SegmentBase() {
  }

  virtual void Cut(const string& sentence, vector<string>& words) const = 0;

  bool ResetSeparators(const string& s) {
    symbols_.clear();
    RuneStrArray runes;
    if (!DecodeRunesInString(s, runes)) {
      XLOG(ERROR) << "decode " << s << " failed";
      return false;
    }
    for (size_t i = 0; i < runes.size(); i++) {
      if (!symbols_.insert(runes[i].rune).second) {
        XLOG(ERROR) << s.substr(runes[i].offset, runes[i].len) << " already exists";
        return false;
      }
    }
    return true;
  }
 protected:
  unordered_set<Rune> symbols_;
}; // class SegmentBase

} // cppjieba

#endif
