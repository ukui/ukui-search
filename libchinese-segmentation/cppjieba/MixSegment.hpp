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

#include <cassert>
#include "MPSegment.hpp"
#include "HMMSegment.hpp"
#include "limonp/StringUtil.hpp"
#include "PosTagger.hpp"

namespace cppjieba {
class MixSegment: public SegmentTagged {
public:
    MixSegment(const DictTrie* dictTrie,
               const HMMModel* model,
               const string& stopWordPath)
        : mpSeg_(dictTrie), hmmSeg_(model) {
        LoadStopWordDict(stopWordPath);
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
            if (words[i].left != words[i].right) {
                res.push_back(GetStringFromRunes(s, words[i].left, words[i].right));
                continue;
            }
            if (mpSeg_.IsUserDictSingleChineseWord(words[i].left->rune)
                    || i == (words.size() - 1)) {//i++后如果是最后一个字符则直接push_back
                res.push_back(GetStringFromRunes(s, words[i].left, words[i].right));
                continue;
            }

            // if mp Get a single one and it is not in userdict, collect it in sequence
            size_t j = i + 1; //当前i字符为单独的字符并且不在用户字典里（i字符不是最后一个字符），直接判定j字符

            while (j < (words.size() - 1) && words[j].left == words[j].right &&
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

    virtual void CutWithSentence(const string& s, RuneStrArray::const_iterator begin, RuneStrArray::const_iterator end, unordered_map<string, KeyWord>& res, bool hmm,
                     size_t) const override {
        vector<WordRange> words;
        vector<WordRange> hmmRes;
        assert(end >= begin);
        if (3 == begin->len or 4 == begin->len) {
            words.reserve(end - begin);
            mpSeg_.CutRuneArray(begin, end, words);
            hmmRes.reserve(words.size());
        } else {
            hmmRes.reserve(end - begin);
        }

        if (words.size() != 0) {//存在中文分词结果
            for (size_t i = 0; i < words.size(); i++) {

                string str = GetStringFromRunes(s, words[i].left, words[i].right);

                if (words[i].left != words[i].right) {
                    if (stopWords_.find(str) != stopWords_.end()) {
                        continue;
                    }
                    res[str].offsets.push_back(words[i].left->offset);
                    res[str].weight += 1.0;
                    continue;
                }

                if (mpSeg_.IsUserDictSingleChineseWord(words[i].left->rune)
                        || i == (words.size() - 1)) {//i++后如果是最后一个字符则直接push_back
                    if (stopWords_.find(str) != stopWords_.end()) {
                        continue;
                    }
                    res[str].offsets.push_back(words[i].left->offset);
                    res[str].weight += 1.0;
                    continue;
                }
                // if mp Get a single one and it is not in userdict, collect it in sequence
                size_t j = i + 1; //当前i字符为单独的字符并且不在用户字典里（i字符不是最后一个字符），直接判定j字符
                bool isLastWordsSingle(false);
                while (j <= (words.size() - 1)
                       && words[j].left == words[j].right
                       && !mpSeg_.IsUserDictSingleChineseWord(words[j].left->rune)) {
                    if (j == (words.size() - 1)) {//最后一个分词结果是单字
                        isLastWordsSingle = true;
                        break;
                    }
                    j++;
                }

                // Cut the sequence with hmm
                assert(j - 1 >= i);
                // TODO
                if (isLastWordsSingle) {
                    hmmSeg_.CutRuneArray(words[i].left, words[j].left + 1, hmmRes);
                } else {
                    hmmSeg_.CutRuneArray(words[i].left, words[j].left, hmmRes);
                }

                //put hmm result to result
                for (size_t k = 0; k < hmmRes.size(); k++) {
                    string hmmStr = GetStringFromRunes(s, hmmRes[k].left, hmmRes[k].right);
                    if (IsSingleWord(hmmStr) || stopWords_.find(hmmStr) != stopWords_.end()) {
                        continue;
                    }
                    res[hmmStr].offsets.push_back(hmmRes[k].left->offset);
                    res[hmmStr].weight += 1.0;
                }

                //clear tmp vars
                hmmRes.clear();

                //let i jump over this piece
                if (isLastWordsSingle) {
                    break;
                }
                i = j - 1;
            }
        } else {//不存在中文分词结果
            for (size_t i = 0; i < (size_t)(end - begin); i++) {
                string str = s.substr((begin+i)->offset, (begin+i)->len);
                res[str].offsets.push_back((begin+i)->offset);
                res[str].weight += 1.0;
            }
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

    void LoadStopWordDict(const string& filePath) {
        ifstream ifs(filePath.c_str());
        if(not ifs.is_open()){
            return ;
        }
        XCHECK(ifs.is_open()) << "open " << filePath << " failed";
        string line ;

        while (getline(ifs, line)) {
            stopWords_.insert(line);
        }

        assert(stopWords_.size());
    }
private:
    unordered_set<string> stopWords_;

    MPSegment mpSeg_;
    HMMSegment hmmSeg_;
    PosTagger tagger_;

}; // class MixSegment

} // namespace cppjieba

