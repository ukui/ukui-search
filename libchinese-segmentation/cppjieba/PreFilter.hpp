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
#ifndef CPPJIEBA_PRE_FILTER_H
#define CPPJIEBA_PRE_FILTER_H

#include "Trie.hpp"
#include "limonp/Logging.hpp"

namespace cppjieba {

class PreFilter {
public:
    //TODO use WordRange instead of Range
    struct Range {
        RuneStrArray::const_iterator begin;
        RuneStrArray::const_iterator end;
    }; // struct Range

    PreFilter(const unordered_set<Rune>& symbols,
              const string& sentence)
        : symbols_(symbols) {
        if(!DecodeRunesInString(sentence, sentence_)) {
            XLOG(ERROR) << "decode failed. ";
        }
        cursor_ = sentence_.begin();
    }
    ~PreFilter() {
    }
    bool HasNext() const {
        return cursor_ != sentence_.end();
    }
    Range Next() {
        Range range;
        range.begin = cursor_;
        while(cursor_ != sentence_.end()) {
            if(IsIn(symbols_, cursor_->rune)) {
                if(range.begin == cursor_) {
                    cursor_ ++;
                }
                range.end = cursor_;
                return range;
            }
            cursor_ ++;
        }
        range.end = sentence_.end();
        return range;
    }
private:
    RuneStrArray::const_iterator cursor_;
    RuneStrArray sentence_;
    const unordered_set<Rune>& symbols_;
}; // class PreFilter

} // namespace cppjieba

#endif // CPPJIEBA_PRE_FILTER_H
