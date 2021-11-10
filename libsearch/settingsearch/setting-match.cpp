/*
 * Copyright (C) 2020, KylinSoft Co., Ltd.
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
 * Authors: sunfengsheng <sunfengsheng@kylinos.cn>
 *
 */
#include "setting-match.h"
#include "file-utils.h"
#include <QProcessEnvironment>
using namespace Zeeker;
SettingsMatch::SettingsMatch(QObject *parent) : QObject(parent) {
    xmlElement();
}

/**
 * @brief SettingsMatch::startMatchApp
 * 返回给页面
 * @param source
 * 获取的字符串
 * @return
 */
QStringList SettingsMatch::startMatchApp(const QString &source) {
    m_sourceText = source;
    QStringList settingList = matching();
    return settingList;
}

/**
 * @brief SettingsMatch::xmlElement
 * 将xml文件内容读到内存
 */
void SettingsMatch::xmlElement() {
    QString chineseIndex;
    QString englishIndex;
    QString path = QProcessEnvironment::systemEnvironment().value("XDG_SESSION_TYPE");
    QString version;
    QFile file(QString::fromLocal8Bit("/usr/share/ukui-control-center/shell/res/search.xml"));
    if(!file.open(QIODevice::ReadOnly)) {
        return;
    }
    QDomDocument doc;
    doc.setContent(&file);
    QDomElement root = doc.documentElement();
    QDomNode node = root.previousSibling();
    node = root.firstChild();

    while(!node.isNull()) {
        QDomElement element = node.toElement();
        QDomNodeList list = element.childNodes();
        //通过子节点数判断xml文件父节点是否有问题
        if (list.count() >= 8 && list.at(6).nodeName() == QString::fromLocal8Bit("EnglishFunc1")) {
            //获取二级菜单英文名
            QString key = list.at(6).toElement().text();
            m_chineseSearchResult = m_chineseSearchList.value(key);
            m_englishSearchResult = m_englishSearchList.value(key);
            for(int i = 0; i < list.count(); ++i) {
                QDomNode n = list.at(i);
                if(n.nodeName()==QString::fromLocal8Bit("Environment")){
                    version=n.toElement().text();
                    if((version=="v101"&&path=="wayland")||(version=="hw990"&&path=="x11")){
                        break;
                    }
                }
                if(n.nodeName() == QString::fromLocal8Bit("ChineseFunc1")) {
                    chineseIndex = n.toElement().text();
                }
                if(n.nodeName() == QString::fromLocal8Bit("ChineseFunc2")) {
                    chineseIndex += QString::fromLocal8Bit("/") + n.toElement().text();
                    m_chineseSearchResult.append(chineseIndex);
                }
                if(n.nodeName() == QString::fromLocal8Bit("EnglishFunc2")) {
                    englishIndex = QString::fromLocal8Bit("/") + n.toElement().text();
                    m_englishSearchResult.append(englishIndex);
                }
            }
            m_chineseSearchList.insert(key, m_chineseSearchResult);
            m_englishSearchList.insert(key, m_englishSearchResult);
            node = node.nextSibling();
        } else {
            qWarning() << "There's something wrong with the xml file's item:" << element.attribute("name");
            node = node.nextSibling();
        }
    }
    file.close();
}

/**
 * @brief SettingsMatch::matching
 * 进行关键字匹配
 * @return
 */
QStringList SettingsMatch::matching() {
    QStringList returnresult;
    QStringList regmatch;
    QString key;
    QStringList pinyinlist;
    QMap<QString, QStringList>::const_iterator i;
    QLocale ql;
    if(ql.language() == QLocale::Chinese) {
        for(i = m_chineseSearchList.constBegin(); i != m_chineseSearchList.constEnd(); ++i) {
            regmatch = *i;
            key = i.key();
            for(int t = 0; t < regmatch.size(); t++) {
                if(m_sourceText == "/")
                    continue;
                QString str = regmatch.at(t);
                pinyinlist = FileUtils::findMultiToneWords(str);
                if(str.contains(m_sourceText)) {
                    str = key + "/" + str;
                    returnresult.append(str);//中文名
                    continue;
                }
                for(int i = 0; i < pinyinlist.size() / 2; i++) {
                    str = regmatch.at(t);
                    QString shouzimu = pinyinlist.at(2 * i + 1); // 中文转首字母
                    if(shouzimu.contains(m_sourceText, Qt::CaseInsensitive)) {
                        str = key + "/" + str;
                        returnresult.append(str);
                        break;
                    }
                    if(m_sourceText.size() < 2)
                        break;
                    QString pinyin = pinyinlist.at(2 * i); // 中文转拼音
                    if(pinyin.contains(m_sourceText, Qt::CaseInsensitive)) {
                        str = key + "/" + str;
                        returnresult.append(str);
                        break;
                    }
                }

            }
        }
    }
    if(ql.language() == QLocale::English) {
        for(i = m_englishSearchList.constBegin(); i != m_englishSearchList.constEnd(); ++i) {
            regmatch = *i;
            key = i.key();
            for(int t = 0; t < regmatch.size(); t++) {
                if(m_sourceText == "/")
                    continue;
                QString str = regmatch.at(t);
                if(str.contains(m_sourceText, Qt::CaseInsensitive)) {
                    str = key + "/" + str;
                    returnresult.append(str);
                }
            }
        }
    }
    return returnresult;
}
