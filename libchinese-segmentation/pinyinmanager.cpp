#include "pinyinmanager.h"
#include <mutex>
PinYinManager * PinYinManager::g_pinYinManager = nullptr;
std::once_flag g_singleFlag;
PinYinManager * PinYinManager::getInstance()
{
    call_once(g_singleFlag, []() {
        g_pinYinManager = new PinYinManager;
    });
    return g_pinYinManager;
}

bool PinYinManager::contains(string &word)
{
    return m_pinYinTrie->contains(word);
}

bool PinYinManager::isMultiTon(string &word)
{
    return m_pinYinTrie->isMultiTone(word);
}

bool PinYinManager::isMultiTon(string word)
{
    return m_pinYinTrie->isMultiTone(word);
}

int PinYinManager::getResults(string word, QStringList &results)
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

PinYinManager::PinYinManager()
{
    const char * const  PINYIN_PATH = "/usr/share/ukui-search/res/dict/pinyinWithoutTone.txt";
    m_pinYinTrie = new cppjieba::PinYinTrie(PINYIN_PATH);
}

PinYinManager::~PinYinManager()
{
    if (m_pinYinTrie){
        delete m_pinYinTrie;
        m_pinYinTrie = nullptr;
    }
}

