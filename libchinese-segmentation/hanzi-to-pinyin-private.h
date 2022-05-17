#ifndef HANZITOPINYINPRIVATE_H
#define HANZITOPINYINPRIVATE_H

#include <QtCore/qglobal.h>
#include "cppjieba/PinYinTrie.hpp"
#include "hanzi-to-pinyin.h"

#define PINYINMANAGER_EXPORT Q_DECL_IMPORT

using namespace std;

class PINYINMANAGER_EXPORT HanZiToPinYinPrivate
{
public:
    HanZiToPinYinPrivate(HanZiToPinYin *parent = nullptr);
    ~HanZiToPinYinPrivate();

public:
    template <typename T>
    bool isMultiTone(T &&t) {return m_pinYinTrie->isMultiTone(std::forward<T>(t));}

    bool contains(string &word);
    int getResults(string word, QStringList &results);

private:
    cppjieba::PinYinTrie *m_pinYinTrie = nullptr;
    HanZiToPinYin *q = nullptr;
};
#endif // HANZITOPINYINPRIVATE_H
