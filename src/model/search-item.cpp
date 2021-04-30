/*
 *
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
 * Authors: zhangjiaping <zhangjiaping@kylinos.cn>
 *
 */
#include "search-item.h"
#include <QDebug>
#include <QFileInfo>


using namespace Zeeker;
SearchItem::SearchItem(QObject *parent) : QObject(parent) {
}

SearchItem::~SearchItem() {
}

/**
 * @brief SearchItem::getIcon 获取一个搜索项的图标
 * @param index 条目索引
 * @return 图标
 */
QIcon SearchItem::getIcon(int index) {
    if(index < 0 || index >= m_pathlist.count())
        return QIcon("");
    switch(m_searchtype) {
    case Settings : //设置项，返回控制面板对应插件的图标
        return FileUtils::getSettingIcon(m_pathlist.at(index), false);
    case Contents:
    case Dirs :
    case Files : //文件，返回文件图标
//        return FileUtils::getFileIcon(QString("file://%1").arg(m_pathlist.at(index)));
        return FileUtils::getFileIcon(QUrl::fromLocalFile(m_pathlist.at(index)).toString());
    case Apps : {//应用，返回应用图标
//            return FileUtils::getAppIcon(m_pathlist.at(index));
        if(m_app_pathlist.length() > index && m_app_pathlist.at(index) == "") {  //未安装，存储的是图标路径
            return QIcon(m_app_iconlist.at(index));
        } else if(m_app_pathlist.length() > index) {  //已安装,存储的是图标名
            if(QIcon::fromTheme(m_app_iconlist.at(index)).isNull()) {
                return QIcon(":/res/icons/desktop.png");
            } else {
                return QIcon::fromTheme(m_app_iconlist.at(index));
            }
        } else {
            return QIcon(":/res/icons/desktop.png");
        }
    }
    case Best : {//最佳匹配，含全部类型，需要自己判断，返回不同类型的图标
        return getBestIcon(index);
    }
    default:
        return QIcon(":/res/icons/edit-find-symbolic.svg");
    }
}

/**
 * @brief SearchItem::getIcon 获取一个搜索项的名称
 * @param index 条目索引
 * @return 名字
 */
QString SearchItem::getName(int index) {
    if(index < 0 || index >= m_pathlist.count())
        return 0;
    switch(m_searchtype) {
    case Settings : //设置项，返回功能点名
        return FileUtils::getSettingName(m_pathlist.at(index));
    case Contents:
    case Dirs :
    case Files : //文件，返回文件名
        return FileUtils::getFileName(m_pathlist.at(index));
    case Apps : {//应用，返回应用名
        QString whole_name = m_pathlist.at(index);
        QString app_name = whole_name.contains("/") ? whole_name.left(whole_name.indexOf("/")) : whole_name;
        return app_name;
    }
    case Best : //最佳匹配，含全部类型，需要自己判断，返回不同类型的名称
        return getBestName(index);
    default:
        return m_pathlist.at(index);
    }
}

/**
 * @brief SearchItem::getBestIcon 获取最佳匹配结果的图标
 * @param index 索引行
 * @return
 */
QIcon SearchItem::getBestIcon(const int &index) {
//    if (m_pathlist.at(index).endsWith(".desktop")) {
//        return FileUtils::getAppIcon(m_pathlist.at(index));
//    } else if (QFileInfo(m_pathlist.at(index)).isFile() || QFileInfo(m_pathlist.at(index)).isDir()) {
//        return FileUtils::getFileIcon(QString("file://%1").arg(m_pathlist.at(index)));
//    } else {
//        return FileUtils::getSettingIcon(m_pathlist.at(index), false);
//    }
    if(m_bestList.isEmpty() || !m_bestList.length() > index) return QIcon::fromTheme("unknown");
    switch(m_bestList.at(index).first) {
    case Apps: {
        return this->m_bestAppIcon;
//                return FileUtils::getAppIcon(m_pathlist.at(index));
    }
    case Settings: {
        return FileUtils::getSettingIcon(m_pathlist.at(index), false);
    }
    default: {
        return FileUtils::getFileIcon(QUrl::fromLocalFile(m_pathlist.at(index)).toString());
    }
    }
}

/**
 * @brief SearchItem::getBestName 获取最佳匹配结果的名称
 * @param index 索引行
 * @return
 */
QString SearchItem::getBestName(const int &index) {
    if(m_bestList.isEmpty() || !m_bestList.length() > index) return "";
    switch(m_bestList.at(index).first) {
    case Apps: {
        QString whole_name = m_bestList.at(index).second;
        QString app_name = whole_name.contains("/") ? whole_name.left(whole_name.indexOf("/")) : whole_name;
        return app_name;
    }
    case Settings: {
        return FileUtils::getSettingName(m_pathlist.at(index));
    }
    default: {
        return FileUtils::getFileName(m_pathlist.at(index));
    }
    }
}

/**
 * @brief SearchItem::setSearchList 存入搜索结果列表
 * @param type 搜索类型
 * @param searchResult 搜索结果
 */
void SearchItem::setSearchList(const int& type, const QStringList& searchResult) {
    m_searchtype = type;
    m_pathlist = searchResult;
}

/**
 * @brief SearchItem::removeItem
 */
void SearchItem::removeItem(QString path) {
    m_pathlist.removeOne(path);
}

int SearchItem::getCurrentSize() {
    return m_pathlist.length();
}

void SearchItem::clear() {
    m_pathlist.clear();
    m_app_pathlist.clear();
    m_app_iconlist.clear();
    m_bestList.clear();
}
