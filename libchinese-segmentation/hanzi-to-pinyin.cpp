#include "hanzi-to-pinyin.h"
#include "hanzi-to-pinyin-private.h"
#include <mutex>

HanZiToPinYin * HanZiToPinYin::g_pinYinManager = nullptr;
std::once_flag g_singleFlag;



bool HanZiToPinYinPrivate::contains(string &word)
{
    return m_pinYinTrie->contains(word);
}

int HanZiToPinYinPrivate::getResults(string word, QStringList &results)
{
    results.clear();
    if (-1 != m_pinYinTrie->getMultiTonResults(word, results)) {
        return 0;
    }
    QString tmp;
    if (-1 != m_pinYinTrie->getSingleTonResult(word, tmp)) {
        results.append(tmp);
        return 0;
    }
    return -1;
}

HanZiToPinYinPrivate::HanZiToPinYinPrivate(HanZiToPinYin *parent) : q(parent)
{
    const char * const  PINYIN_PATH = "/usr/share/ukui-search/res/dict/pinyinWithoutTone.txt";
    m_pinYinTrie = new cppjieba::PinYinTrie(PINYIN_PATH);
}

HanZiToPinYinPrivate::~HanZiToPinYinPrivate()
{
    if (m_pinYinTrie){
        delete m_pinYinTrie;
        m_pinYinTrie = nullptr;
    }
}

HanZiToPinYin * HanZiToPinYin::getInstance()
{
    call_once(g_singleFlag, []() {
        g_pinYinManager = new HanZiToPinYin;
    });
    return g_pinYinManager;
}

bool HanZiToPinYin::contains(string &word)
{
    return d->contains(word);
}

bool HanZiToPinYin::isMultiTone(string &word)
{
    return d->isMultiTone(word);
}

bool HanZiToPinYin::isMultiTone(string &&word)
{
    return d->isMultiTone(word);
}

bool HanZiToPinYin::isMultiTone(const string &word)
{
    return d->isMultiTone(word);
}

bool HanZiToPinYin::isMultiTone(const string &&word)
{
    return d->isMultiTone(word);
}

int HanZiToPinYin::getResults(string word, QStringList &results)
{
    return d->getResults(word, results);
}

HanZiToPinYin::HanZiToPinYin() : d(new HanZiToPinYinPrivate)
{
}
