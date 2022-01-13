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
#pragma once

#include "limonp/Logging.hpp"
#include <unordered_set>
#include "Unicode.hpp"

namespace cppjieba {

class PreFilter {
public:
    PreFilter(const std::unordered_set<Rune>& symbols,
              const string& sentence)
        : symbols_(symbols) {
        if (!DecodeRunesInString(sentence, sentence_)) {
            XLOG(ERROR) << "decode failed. "<<sentence;
        }

        cursor_ = sentence_.begin();
    }
    ~PreFilter() {
    }
    bool HasNext() const {
        return cursor_ != sentence_.end();
    }
    bool Next(WordRange& wordRange) {

        if (cursor_ == sentence_.end()) {
            return false;
        }

        wordRange.left = cursor_;

        while (cursor_->rune == 0x20 && cursor_ != sentence_.end()) {
            cursor_++;
        }

        if (cursor_ == sentence_.end()) {
            wordRange.right = cursor_;
            return true;
        }

        while (++cursor_ != sentence_.end()) {
            if (cursor_->rune == 0x20) {
                wordRange.right = cursor_;
                return true;
            }
        }

        wordRange.right = sentence_.end();
        return true;
    }

    bool Next(WordRange& wordRange, bool& isNull) {
        isNull = false;
        if (cursor_ == sentence_.end()) {
            return false;
        }

        wordRange.left = cursor_;
        if (cursor_->rune == 0x20) {
            while (cursor_ != sentence_.end()) {
                if (cursor_->rune != 0x20) {
                    if (wordRange.left == cursor_) {
                        cursor_ ++;
                    }
                    wordRange.right = cursor_;
                    isNull = true;
                    return true;
                }
                cursor_ ++;
            }
        }

        int max_num = 0;
        uint32_t utf8_num = cursor_->len;

        while (cursor_ != sentence_.end()) {
            if (cursor_->rune == 0x20) {
                if (wordRange.left == cursor_) {
                    cursor_ ++;
                }

                wordRange.right = cursor_;
                return true;
            }

            cursor_ ++;
            max_num++;
            if (max_num >= 1024 or cursor_->len != utf8_num) { //todo 防止一次性传入过多字节，暂定限制为1024个字
                wordRange.right = cursor_;
                return true;
            }
        }

        wordRange.right = sentence_.end();
        return true;
    }

    WordRange Next() {
        WordRange range(cursor_, cursor_);

        while (cursor_ != sentence_.end()) {
            //if (IsIn(symbols_, cursor_->rune)) {
            if (cursor_->rune == 0x20) {
                if (range.left == cursor_) {
                    cursor_ ++;
                }

                range.right = cursor_;
                return range;
            }

            cursor_ ++;
        }

        range.right = sentence_.end();
        return range;
    }
private:
    RuneStrArray::const_iterator cursor_;
    RuneStrArray sentence_;
    const std::unordered_set<Rune>& symbols_;
}; // class PreFilter

} // namespace cppjieba

