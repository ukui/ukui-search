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

#include <memory>
#include "QuerySegment.hpp"
#include "KeywordExtractor.hpp"

namespace cppjieba {

class Jieba {
public:
    Jieba(const string& dict_path,
          const string& model_path,
          const string& user_dict_path,
          const string& idfPath = "",
          const string& stopWordPath = "",
          const string& dat_cache_path = "")
        : dict_trie_(dict_path, user_dict_path, dat_cache_path),
          model_(model_path),
          mp_seg_(&dict_trie_),
          hmm_seg_(&model_),
          mix_seg_(&dict_trie_, &model_, stopWordPath),
          full_seg_(&dict_trie_),
          query_seg_(&dict_trie_, &model_, stopWordPath),
          extractor(&dict_trie_, &model_, idfPath, dat_cache_path,stopWordPath){ }
    ~Jieba() { }

    void Cut(const string& sentence, vector<string>& words, bool hmm = true) const {
        mix_seg_.CutToStr(sentence, words, hmm);
    }
    void Cut(const string& sentence, vector<Word>& words, bool hmm = true) const {
        mix_seg_.CutToWord(sentence, words, hmm);
    }
    void CutAll(const string& sentence, vector<string>& words) const {
        full_seg_.CutToStr(sentence, words);
    }
    void CutAll(const string& sentence, vector<Word>& words) const {
        full_seg_.CutToWord(sentence, words);
    }
    void CutForSearch(const string& sentence, vector<string>& words, bool hmm = true) const {
        query_seg_.CutToStr(sentence, words, hmm);
    }
    void CutForSearch(const string& sentence, vector<Word>& words, bool hmm = true) const {
        query_seg_.CutToWord(sentence, words, hmm);
    }
    void CutHMM(const string& sentence, vector<string>& words) const {
        hmm_seg_.CutToStr(sentence, words);
    }
    void CutHMM(const string& sentence, vector<Word>& words) const {
        hmm_seg_.CutToWord(sentence, words);
    }
    void CutSmall(const string& sentence, vector<string>& words, size_t max_word_len) const {
        mp_seg_.CutToStr(sentence, words, false, max_word_len);
    }
    void CutSmall(const string& sentence, vector<Word>& words, size_t max_word_len) const {
        mp_seg_.CutToWord(sentence, words, false, max_word_len);
    }

    void Tag(const string& sentence, vector<pair<string, string> >& words) const {
        mix_seg_.Tag(sentence, words);
    }
    string LookupTag(const string &str) const {
        return mix_seg_.LookupTag(str);
    }
    bool Find(const string& word) {
        return nullptr != dict_trie_.Find(word);
    }

    void ResetSeparators(const string& s) {
        //TODO
        mp_seg_.ResetSeparators(s);
        hmm_seg_.ResetSeparators(s);
        mix_seg_.ResetSeparators(s);
        full_seg_.ResetSeparators(s);
        query_seg_.ResetSeparators(s);
    }

    const DictTrie* GetDictTrie() const {
        return &dict_trie_;
    }

    const HMMModel* GetHMMModel() const {
        return &model_;
    }

private:
    DictTrie dict_trie_;
    HMMModel model_;

    // They share the same dict trie and model
    MPSegment mp_seg_;
    HMMSegment hmm_seg_;
    MixSegment mix_seg_;
    FullSegment full_seg_;
    QuerySegment query_seg_;

public:
    KeywordExtractor extractor;
}; // class Jieba

} // namespace cppjieba

