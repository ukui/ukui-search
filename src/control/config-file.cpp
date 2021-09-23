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
#include "config-file.h"
#include "common.h"

using namespace Zeeker;
static ConfigFile *config_file_Class = nullptr;

ConfigFile *ConfigFile::getInstance() {
    if(!config_file_Class) {
        config_file_Class = new ConfigFile;
    }
    return config_file_Class;
}
ConfigFile::ConfigFile() {
    m_settings = new QSettings(HOMEPAGE_SETTINGS, QSettings::IniFormat);
}

ConfigFile::~ConfigFile() {
    if(m_settings){
        delete m_settings;
    }
}

bool ConfigFile::writeCommonly(QString message) {
    for(QString appPath: allAppPath) {
        if(message.startsWith(appPath)) {
            m_settings->beginGroup("Commonly");
            QStringList quickly = m_settings->allKeys();
            if(quickly.contains(message.mid(1, message.length() - 1))) {
                m_settings->setValue(message, m_settings->value(message).toInt() + 1);
            } else {
                m_settings->setValue(message, 1);
            }
            m_settings->endGroup();
            return true;
        }
    }
    return false;
}

QStringList ConfigFile::readCommonly() {
    QStringList returnlist;
    QMap<QString, int> quicklycount;

    m_settings->beginGroup("Commonly");
    QStringList Commonly = m_settings->allKeys();
    for(int i = 0; i < Commonly.size(); i++) {
        quicklycount.insert(Commonly.at(i), m_settings->value(Commonly.at(i)).toInt());
    }
    m_settings->endGroup();

    QMap<QString, int>::iterator iter = quicklycount.begin();
    QVector<QPair<QString, int>> vec;
    while(iter != quicklycount.end()) {
        vec.push_back(qMakePair(iter.key(), iter.value()));
        iter++;
    }
    qSort(vec.begin(), vec.end(), [](const QPair<QString, int> &l, const QPair<QString, int> &r) {
        return (l.second > r.second);
    });
    for(int j = 0; j < vec.size(); j++) {
        returnlist.append("/" + vec.at(j).first);
    }
    return returnlist.mid(0, 5);
}
void ConfigFile::removeCommonly(QString path)
{
    m_settings->beginGroup("Commonly");
    m_settings->remove(path);
    m_settings->endGroup();
}

bool ConfigFile::writeRecently(QString message) {
    m_settings->beginGroup("Recently");
    QStringList recently = m_settings->value("Recently").toStringList();

    if(recently.contains(message)) {
        recently.removeOne(message);
    }
    recently.insert(0, message);

    if(m_settings->value("Recently").toStringList().length() >= 10) {
        m_settings->setValue("Recently", QStringList(recently.mid(0, 10)));
    } else {
        m_settings->setValue("Recently", recently);
    }
    m_settings->endGroup();
    return true;
}

QStringList ConfigFile::readRecently() {
    m_settings->beginGroup("Recently");
    QStringList recently = m_settings->value("Recently").toStringList();
    m_settings->endGroup();
    return recently.mid(0, 4);
}

void ConfigFile::removeRecently(QString path)
{
    m_settings->beginGroup("Recently");
    QStringList recentlyList = m_settings->value("Recently").toStringList();
    recentlyList.removeAll(path);
    m_settings->setValue("Recently",recentlyList);
    m_settings->endGroup();
}

bool ConfigFile::writeConfig(QString message) {
    bool isWriteCommonlyDone = writeCommonly(message);
    bool isWriteRecentlyDone = writeRecently(message);
    return (isWriteCommonlyDone || isWriteRecentlyDone);
}

QMap<QString, QStringList> ConfigFile::readConfig() {
    QStringList templist = readRecently();
    for(QString path: templist) {
        if(!itemExist(path)) {
            removeConfig(path);
        }
    }
    QMap<QString, QStringList> returnresult;
    returnresult.insert("Commonly", readCommonly());
    returnresult.insert("Recently", readRecently());
    return returnresult;
}

void ConfigFile::removeConfig(QString path)
{
    removeCommonly(path);
    removeRecently(path);
}

bool ConfigFile::itemExist(QString path)
{
    QFileInfo file(path);
    if(file.exists() || path.left(1) != "/") {    //路径存在或为设置项
        return true;
    }
    return false;
}

void ConfigFile::receiveMessage(QString message) {
    QFile file(HOMEPAGE_SETTINGS);
    if(!file.exists()) {
        file.open(QIODevice::ReadWrite | QIODevice::Text);
        file.close();
    }
    readConfig();//页面调用
    writeConfig(message);
}
