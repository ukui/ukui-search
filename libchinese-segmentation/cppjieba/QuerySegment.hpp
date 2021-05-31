#pragma once

#include <algorithm>
#include <set>
#include <cassert>
#include "limonp/Logging.hpp"
#include "DictTrie.hpp"
#include "SegmentBase.hpp"
#include "FullSegment.hpp"
#include "MixSegment.hpp"
#include "Unicode.hpp"
#include "DictTrie.hpp"

namespace cppjieba {
class QuerySegment: public SegmentBase {
public:
    QuerySegment(const DictTrie* dictTrie, const HMMModel* model)
        : mixSeg_(dictTrie, model), trie_(dictTrie) {
    }
    ~QuerySegment() {
    }

    virtual void Cut(RuneStrArray::const_iterator begin, RuneStrArray::const_iterator end, vector<WordRange>& res, bool hmm,
                     size_t) const override {
        //use mix Cut first
        vector<WordRange> mixRes;
        mixSeg_.CutRuneArray(begin, end, mixRes, hmm);

        vector<WordRange> fullRes;

        for (vector<WordRange>::const_iterator mixResItr = mixRes.begin(); mixResItr != mixRes.end(); mixResItr++) {
            if (mixResItr->Length() > 2) {
                for (size_t i = 0; i + 1 < mixResItr->Length(); i++) {
                    string text = EncodeRunesToString(mixResItr->left + i, mixResItr->left + i + 2);

                    if (trie_->Find(text) != NULL) {
                        WordRange wr(mixResItr->left + i, mixResItr->left + i + 1);
                        res.push_back(wr);
                    }
                }
            }

            if (mixResItr->Length() > 3) {
                for (size_t i = 0; i + 2 < mixResItr->Length(); i++) {
                    string text = EncodeRunesToString(mixResItr->left + i, mixResItr->left + i + 3);

                    if (trie_->Find(text) != NULL) {
                        WordRange wr(mixResItr->left + i, mixResItr->left + i + 2);
                        res.push_back(wr);
                    }
                }
            }

            res.push_back(*mixResItr);
        }
    }

    virtual void CutWithSentence(const string& s, RuneStrArray::const_iterator begin, RuneStrArray::const_iterator end, vector<string>& res, bool hmm,
                     size_t) const override {

    }

private:
    bool IsAllAscii(const RuneArray& s) const {
        for (size_t i = 0; i < s.size(); i++) {
            if (s[i] >= 0x80) {
                return false;
            }
        }

        return true;
    }
    MixSegment mixSeg_;
    const DictTrie* trie_;
}; // QuerySegment

} // namespace cppjieba

