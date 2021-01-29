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
#ifndef CPPJIEBA_POS_TAGGING_H
#define CPPJIEBA_POS_TAGGING_H

#include "limonp/StringUtil.hpp"
#include "SegmentTagged.hpp"
#include "DictTrie.hpp"

namespace cppjieba {
using namespace limonp;

static const char* const POS_M = "m";
static const char* const POS_ENG = "eng";
static const char* const POS_X = "x";

class PosTagger {
 public:
  PosTagger() {
  }
  ~PosTagger() {
  }

  bool Tag(const string& src, vector<pair<string, string> >& res, const SegmentTagged& segment) const {
    vector<string> CutRes;
    segment.Cut(src, CutRes);

    for (vector<string>::iterator itr = CutRes.begin(); itr != CutRes.end(); ++itr) {
      res.push_back(make_pair(*itr, LookupTag(*itr, segment)));
    }
    return !res.empty();
  }

  string LookupTag(const string &str, const SegmentTagged& segment) const {
    const DictUnit *tmp = NULL;
    RuneStrArray runes;
    const DictTrie * dict = segment.GetDictTrie();
    assert(dict != NULL);
      if (!DecodeRunesInString(str, runes)) {
        XLOG(ERROR) << "Decode failed.";
        return POS_X;
      }
      tmp = dict->Find(runes.begin(), runes.end());
      if (tmp == NULL || tmp->tag.empty()) {
        return SpecialRule(runes);
      } else {
        return tmp->tag;
      }
  }

 private:
  const char* SpecialRule(const RuneStrArray& unicode) const {
    size_t m = 0;
    size_t eng = 0;
    for (size_t i = 0; i < unicode.size() && eng < unicode.size() / 2; i++) {
      if (unicode[i].rune < 0x80) {
        eng ++;
        if ('0' <= unicode[i].rune && unicode[i].rune <= '9') {
          m++;
        }
      }
    }
    // ascii char is not found
    if (eng == 0) {
      return POS_X;
    }
    // all the ascii is number char
    if (m == eng) {
      return POS_M;
    }
    // the ascii chars contain english letter
    return POS_ENG;
  }

}; // class PosTagger

} // namespace cppjieba

#endif
