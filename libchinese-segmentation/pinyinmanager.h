#ifndef PINYINMANAGER_H
#define PINYINMANAGER_H

#include <QtCore/qglobal.h>
#include "cppjieba/PinYinTrie.hpp"

#define PINYINMANAGER_EXPORT Q_DECL_IMPORT

using namespace std;

class PINYINMANAGER_EXPORT PinYinManager
{
public:
    static PinYinManager * getInstance();

public:
    bool contains(string &word);
    bool isMultiTon(string &word);
    bool isMultiTon(string word);

    int getResults(string word, QStringList &results);

protected:
    PinYinManager();
    ~PinYinManager();

private:
    static PinYinManager *g_pinYinManager;
    cppjieba::PinYinTrie *m_pinYinTrie = nullptr;

};

#endif // PINYINMANAGER_H
