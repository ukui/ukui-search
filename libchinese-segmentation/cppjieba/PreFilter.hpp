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
    WordRange Next() {
        WordRange range(cursor_, cursor_);

        while (cursor_ != sentence_.end()) {
            if (IsIn(symbols_, cursor_->rune)) {
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

