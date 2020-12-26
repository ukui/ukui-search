#include "chinesecharacterstopinyin.h"

chineseCharactersToPinyin::chineseCharactersToPinyin(QObject *parent) : QObject(parent)
{
    map = loadHanziTable("://index/pinyinWithoutTone.txt");
}


