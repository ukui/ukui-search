#pragma once

#include <algorithm>
#include <set>
#include <cassert>
#include "limonp/Logging.hpp"
#include "DictTrie.hpp"
#include "SegmentTagged.hpp"
#include "PosTagger.hpp"

namespace cppjieba {

class MPSegment: public SegmentTagged {
public:
    MPSegment(const DictTrie* dictTrie)
        : dictTrie_(dictTrie) {
        assert(dictTrie_);
    }
    ~MPSegment() { }

    virtual void Cut(RuneStrArray::const_iterator begin,
                     RuneStrArray::const_iterator end,
                     vector<WordRange>& words,
                     bool, size_t max_word_len) const override {
//        vector<DatDag> dags;
//        dictTrie_->Find(begin, end, dags, max_word_len);//依据DAG词典生成DAG--jxx
//        CalcDP(dags);//动态规划（Dynamic Programming，DP），根据DAG计算最优动态规划路径--jxx
//        CutByDag(begin, end, dags, words);//依据DAG最优路径分词--jxx
        dictTrie_->Find(begin, end, words, max_word_len);
    }

    virtual void CutWithSentence(const string& s, RuneStrArray::const_iterator begin, RuneStrArray::const_iterator end, vector<string>& res, bool hmm,
                     size_t) const override {

    }
    virtual void CutWithSentence(const string& s, RuneStrArray::const_iterator begin, RuneStrArray::const_iterator end, unordered_map<string, KeyWord>& res, bool hmm,
                     size_t) const override {

    }
    const DictTrie* GetDictTrie() const override {
        return dictTrie_;
    }

    bool Tag(const string& src, vector<pair<string, string> >& res) const override {
        return tagger_.Tag(src, res, *this);
    }

    bool IsUserDictSingleChineseWord(const Rune& value) const {
        return dictTrie_->IsUserDictSingleChineseWord(value);
    }
private:
/*
    void CalcDP(vector<DatDag>& dags) const {
        double val(0);
        for (auto rit = dags.rbegin(); rit != dags.rend(); rit++) {
            rit->max_next = -1;
            rit->max_weight = MIN_DOUBLE;

            for (const auto & it : rit->nexts) {
                const auto nextPos = it.first;
                val = dictTrie_->GetMinWeight();

                if (nullptr != it.second) {
                    val = it.second->weight;
                }

                if (nextPos  < dags.size()) {
                    val += dags[nextPos].max_weight;
                }

                if ((nextPos <= dags.size()) && (val > rit->max_weight)) {
                    rit->max_weight = val;
                    rit->max_next = nextPos;
                }
            }
        }
    }
*/
/*  倒叙方式重写CalcDP函数，初步测试未发现问题*/
    void CalcDP(vector<DatDag>& dags) const {
        double val(0);
        size_t size = dags.size();

        for (size_t i = 0; i < size; i++) {
            dags[size - 1 - i].max_next = -1;
            dags[size - 1 - i].max_weight = MIN_DOUBLE;

            for (const auto & it : dags[size - 1 - i].nexts) {
                const auto nextPos = it.first;
                val = dictTrie_->GetMinWeight();

                if (nullptr != it.second) {
                    val = it.second->weight;
                }

                if (nextPos  < dags.size()) {
                    val += dags[nextPos].max_weight;
                }

                if ((nextPos <= dags.size()) && (val > dags[size - 1 - i].max_weight)) {
                    dags[size - 1 - i].max_weight = val;
                    dags[size - 1 - i].max_next = nextPos;
                }
            }
        }
    }

    void CutByDag(RuneStrArray::const_iterator begin,
                  RuneStrArray::const_iterator,
                  const vector<DatDag>& dags,
                  vector<WordRange>& words) const {

        for (size_t i = 0; i < dags.size();) {
            const auto next = dags[i].max_next;
            assert(next > i);
            assert(next <= dags.size());
            WordRange wr(begin + i, begin + next - 1);
            words.push_back(wr);
            i = next;
        }
    }

    const DictTrie* dictTrie_;
    PosTagger tagger_;

}; // class MPSegment

} // namespace cppjieba

