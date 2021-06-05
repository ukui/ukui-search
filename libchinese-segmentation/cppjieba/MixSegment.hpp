#pragma once

#include <cassert>
#include "MPSegment.hpp"
#include "HMMSegment.hpp"
#include "limonp/StringUtil.hpp"
#include "PosTagger.hpp"

namespace cppjieba {
class MixSegment: public SegmentTagged {
public:
    MixSegment(const DictTrie* dictTrie, const HMMModel* model)
        : mpSeg_(dictTrie), hmmSeg_(model) {
    }
    ~MixSegment() {}

    virtual void Cut(RuneStrArray::const_iterator begin, RuneStrArray::const_iterator end, vector<WordRange>& res, bool hmm,
                     size_t) const override {
        if (!hmm) {
            mpSeg_.CutRuneArray(begin, end, res);
            return;
        }

        vector<WordRange> words;
        assert(end >= begin);
        words.reserve(end - begin);
        mpSeg_.CutRuneArray(begin, end, words);

        vector<WordRange> hmmRes;
        hmmRes.reserve(end - begin);

        for (size_t i = 0; i < words.size(); i++) {
            //if mp Get a word, it's ok, put it into result
            if (words[i].left != words[i].right || (words[i].left == words[i].right &&
                                                    mpSeg_.IsUserDictSingleChineseWord(words[i].left->rune))) {
                res.push_back(words[i]);
                continue;
            }

            // if mp Get a single one and it is not in userdict, collect it in sequence
            size_t j = i;

            while (j < words.size() && words[j].left == words[j].right &&
                   !mpSeg_.IsUserDictSingleChineseWord(words[j].left->rune)) {
                j++;
            }

            // Cut the sequence with hmm
            assert(j - 1 >= i);
            // TODO
            hmmSeg_.CutRuneArray(words[i].left, words[j - 1].left + 1, hmmRes);

            //put hmm result to result
            for (size_t k = 0; k < hmmRes.size(); k++) {
                res.push_back(hmmRes[k]);
            }

            //clear tmp vars
            hmmRes.clear();

            //let i jump over this piece
            i = j - 1;
        }
    }

    virtual void CutWithSentence(const string& s, RuneStrArray::const_iterator begin, RuneStrArray::const_iterator end, vector<string>& res, bool hmm,
                     size_t) const override {
        //目前hmm默认开启，后期如有需要关闭再修改--jxx20210519
//        if (!hmm) {
//            mpSeg_.CutRuneArray(begin, end, res);
//            return;
//        }

        vector<WordRange> words;
        assert(end >= begin);
        words.reserve(end - begin);
        mpSeg_.CutRuneArray(begin, end, words);

        vector<WordRange> hmmRes;
        hmmRes.reserve(end - begin);

        for (size_t i = 0; i < words.size(); i++) {
            //if mp Get a word, it's ok, put it into result
            if (words[i].left != words[i].right || (words[i].left == words[i].right &&
                                                    mpSeg_.IsUserDictSingleChineseWord(words[i].left->rune))) {
                res.push_back(GetStringFromRunes(s, words[i].left, words[i].right));
                continue;
            }

            // if mp Get a single one and it is not in userdict, collect it in sequence
            size_t j = i;

            while (j < words.size() && words[j].left == words[j].right &&
                   !mpSeg_.IsUserDictSingleChineseWord(words[j].left->rune)) {
                j++;
            }

            // Cut the sequence with hmm
            assert(j - 1 >= i);
            // TODO
            hmmSeg_.CutRuneArray(words[i].left, words[j - 1].left + 1, hmmRes);

            //put hmm result to result
            for (size_t k = 0; k < hmmRes.size(); k++) {
                res.push_back(GetStringFromRunes(s, hmmRes[k].left, hmmRes[k].right));
            }

            //clear tmp vars
            hmmRes.clear();

            //let i jump over this piece
            i = j - 1;
        }
    }

    const DictTrie* GetDictTrie() const override {
        return mpSeg_.GetDictTrie();
    }

    bool Tag(const string& src, vector<pair<string, string> >& res) const override {
        return tagger_.Tag(src, res, *this);
    }

    string LookupTag(const string &str) const {
        return tagger_.LookupTag(str, *this);
    }

private:
    MPSegment mpSeg_;
    HMMSegment hmmSeg_;
    PosTagger tagger_;

}; // class MixSegment

} // namespace cppjieba

