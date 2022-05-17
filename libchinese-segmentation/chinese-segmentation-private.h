#ifndef CHINESESEGMENTATIONPRIVATE_H
#define CHINESESEGMENTATIONPRIVATE_H

#include "chinese-segmentation.h"
#include "cppjieba/Jieba.hpp"
#include "cppjieba/KeywordExtractor.hpp"

class ChineseSegmentationPrivate
{
public:
    explicit ChineseSegmentationPrivate(ChineseSegmentation *parent = nullptr);
    ~ChineseSegmentationPrivate();
    vector<KeyWord> callSegment(const string& sentence);

    vector<string> callMixSegmentCutStr(const string& sentence);
    vector<Word> callMixSegmentCutWord(const string& sentence);
    string lookUpTagOfWord(const string& word);
    vector<pair<string, string>> getTagOfWordsInSentence(const string &sentence);

    vector<Word> callFullSegment(const string& sentence);

    vector<Word> callQuerySegment(const string& sentence);

    vector<Word> callHMMSegment(const string& sentence);

    vector<Word> callMPSegment(const string& sentence);

private:
    cppjieba::Jieba *m_jieba;
    ChineseSegmentation *q = nullptr;
};

#endif // CHINESESEGMENTATIONPRIVATE_H
