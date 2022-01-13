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

#include <algorithm>
#include <set>
#include <cassert>
#include "limonp/Logging.hpp"
#include "DictTrie.hpp"
#include "SegmentBase.hpp"
#include "Unicode.hpp"

namespace cppjieba {
class FullSegment: public SegmentBase {
public:
    FullSegment(const DictTrie* dictTrie)
        : dictTrie_(dictTrie) {
        assert(dictTrie_);
    }
    ~FullSegment() { }

    virtual void Cut(RuneStrArray::const_iterator begin,
                     RuneStrArray::const_iterator end,
                     vector<WordRange>& res, bool, size_t) const override {
        assert(dictTrie_);
        vector<struct DatDag> dags;
        dictTrie_->Find(begin, end, dags);
        size_t max_word_end_pos = 0;

        for (size_t i = 0; i < dags.size(); i++) {
            for (const auto & kv : dags[i].nexts) {
                const size_t nextoffset = kv.first - 1;
                assert(nextoffset < dags.size());
                const auto wordLen = nextoffset - i + 1;
                const bool is_not_covered_single_word = ((dags[i].nexts.size() == 1) && (max_word_end_pos <= i));
                const bool is_oov = (nullptr == kv.second); //Out-of-Vocabulary

                if ((is_not_covered_single_word) || ((not is_oov) && (wordLen >= 2))) {
                    WordRange wr(begin + i, begin + nextoffset);
                    res.push_back(wr);
                }

                max_word_end_pos = max(max_word_end_pos, nextoffset + 1);
            }
        }
    }

    virtual void CutWithSentence(const string& s, RuneStrArray::const_iterator begin, RuneStrArray::const_iterator end, vector<string>& res, bool hmm,
                     size_t) const override {

    }
    virtual void CutWithSentence(const string& s, RuneStrArray::const_iterator begin, RuneStrArray::const_iterator end, unordered_map<string, KeyWord>& res, bool hmm,
                     size_t) const override {

    }
private:
    const DictTrie* dictTrie_;
};
}

