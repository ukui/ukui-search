#ifndef HANZITOPINYIN_H
#define HANZITOPINYIN_H

#include <QtCore/qglobal.h>
//#include "cppjieba/PinYinTrie.hpp"
#include <QStringList>
#define PINYINMANAGER_EXPORT Q_DECL_IMPORT

using namespace std;

class HanZiToPinYinPrivate;
class PINYINMANAGER_EXPORT HanZiToPinYin
{
public:
    static HanZiToPinYin * getInstance();

public:
    /**
     * @brief HanZiToPinYin::isMultiTone 判断是否为多音字（只支持单字）
     * @param word 要判断的字
     * @return bool 不是多音字或不是单字返回false
     */
    bool isMultiTone(string &word);
    bool isMultiTone(string &&word);
    bool isMultiTone(const string &word);
    bool isMultiTone(const string &&word);

    /**
     * @brief HanZiToPinYin::contains 查询某个字是否有拼音（是否在数据库包含，只支持单字）
     * @param word 要查询的字
     * @return bool 数据库不包含或不是单字返回false
     */
    bool contains(string &word);

    /**
     * @brief HanZiToPinYin::getResults 获取某个字的拼音（只支持单字）
     * @param word 要获取拼音的字
     * @param results word的拼音列表（有可能多音字），每次调用results会清空
     * @return int 获取到返回0，否则返回-1
     */
    int getResults(string word, QStringList &results);

protected:
    HanZiToPinYin();
    ~HanZiToPinYin();
    HanZiToPinYin(const HanZiToPinYin&) = delete;
    HanZiToPinYin& operator =(const HanZiToPinYin&) = delete;
private:
    static HanZiToPinYin *g_pinYinManager;
    HanZiToPinYinPrivate *d = nullptr;
};

#endif // PINYINMANAGER_H
