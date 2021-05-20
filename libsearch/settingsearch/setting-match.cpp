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
    QString ChineseIndex;
    QString EnglishIndex;
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
        QString key = element.attribute("name");;
        m_chine_searchResult = m_chine_searchList.value(key);
        m_English_searchResult = m_English_searchList.value(key);
        QDomNodeList list = element.childNodes();
        for(int i = 0; i < list.count(); ++i) {
            QDomNode n = list.at(i);
            if(n.nodeName()==QString::fromLocal8Bit("Environment")){
                version=n.toElement().text();
                if((version=="v101"&&path=="wayland")||(version=="hw990"&&path=="x11")){
                    break;
                }
            }
            if(n.nodeName() == QString::fromLocal8Bit("ChinesePlugin")) {
                ChineseIndex = n.toElement().text();
            }
            if(n.nodeName() == QString::fromLocal8Bit("ChineseFunc")) {
                ChineseIndex += QString::fromLocal8Bit("/") + n.toElement().text();
                m_chine_searchResult.append(ChineseIndex);
            }
            if(n.nodeName() == QString::fromLocal8Bit("EnglishFunc")) {
                EnglishIndex = QString::fromLocal8Bit("/") + n.toElement().text();
                m_English_searchResult.append(EnglishIndex);
            }
        }
        m_chine_searchList.insert(key, m_chine_searchResult);
        m_English_searchList.insert(key, m_English_searchResult);
        node = node.nextSibling();
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
        for(i = m_chine_searchList.constBegin(); i != m_chine_searchList.constEnd(); ++i) {
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
        for(i = m_English_searchList.constBegin(); i != m_English_searchList.constEnd(); ++i) {
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
