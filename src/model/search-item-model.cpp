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
#include "search-item-model.h"
#include <QDebug>

using namespace Zeeker;
SearchItemModel::SearchItemModel(QObject *parent) : QAbstractItemModel(parent) {

}
SearchItemModel::~SearchItemModel() {

}

/**
 * @brief FileItemModel::index
 * @param rowa
 * @param column
 * @param parent
 * @return
 */
QModelIndex SearchItemModel::index(int row, int column, const QModelIndex &parent) const {
    if(row < 0 || row > m_item->m_pathlist.count() - 1)
        return QModelIndex();
    return createIndex(row, column, m_item);
}

/**
 * @brief FileItemModel::parent
 * @param child
 * @return
 */
QModelIndex SearchItemModel::parent(const QModelIndex &child) const {
    return QModelIndex();
}


/**
 * @brief SearchItemModel::rowCount 重写的model行数函数
 * @param index 条目的索引
 * @return model显示的行数
 */
int SearchItemModel::rowCount(const QModelIndex& index) const {
    return index.isValid() ? 0 : m_item->m_pathlist.count();
}

/**
 * @brief SearchItemModel::columnCount 重写的model列数函数
 * @param index 条目的索引
 * @return model显示的列数
 */

int SearchItemModel::columnCount(const QModelIndex& index) const {
    return index.isValid() ? 0 : 1;
}

/**
 * @brief SearchItemModel::headerData filemodel::columnCount 重写的model标头函数
 * @param section 列
 * @param orientation 显示方式
 * @param role 显示内容类型
 * @return 标头数据
 */
//QVariant SearchItemModel::headerData(int section,Qt::Orientation orientation ,int role) const {
//    return tr("");
////    return QAbstractItemModel::headerData(section, orientation, role);
//}

/**
 * @brief SearchItemModel::data model每条条目的数据，有显示内容，图片
 * @param index 条目索引
 * @param role 显示内容的类型
 * @return 显示内容数据
 */

using namespace Zeeker;
QVariant SearchItemModel::data(const QModelIndex &index, int role) const {
    if(!index.isValid())
        return QVariant();
    switch(role) {
    case Qt::DecorationRole: {
        return m_item->getIcon(index.row());
    }
    case Qt::DisplayRole: {
        return QVariant(m_item->getName(index.row()));
    }
    default:
        return QVariant();
    }
    return QVariant();
}

bool SearchItemModel::insertRows(int row, int count, const QModelIndex &parent) {
    this->beginInsertRows(parent, row, count);
    this->endInsertRows();
    return true;
}

/**
 * @brief SearchItemModel::setItem 传入存储数据的item
 * @param item
 */
void SearchItemModel::setItem(SearchItem * item) {
    m_item = item;
}

/**
 * @brief SearchItemModel::appendItem
 */
void SearchItemModel::appendItem(QString path) {
    this->beginResetModel();
    m_item->m_pathlist << path;
    this->endResetModel();
    //    this->insertRow(rowCount(QModelIndex()) - 1);
}

/**
 * @brief SearchItemModel::setList 直接以列表形式添加搜索结果
 * @param list
 */
void SearchItemModel::setList(QStringList list) {
    this->beginResetModel();
    m_item->m_pathlist = list;
    this->endResetModel();
}

void SearchItemModel::setAppList(const QStringList &pathlist, const QStringList &iconlist) {
    this->beginResetModel();
    m_item->m_app_pathlist = pathlist;
    m_item->m_app_iconlist = iconlist;
    this->endResetModel();
}

/**
 * @brief SearchItemModel::insertBestItem 向最佳匹配列表添加一项
 * @param pair <类型，路径或名称>
 */
void SearchItemModel::appendBestItem(const QPair<int, QString> & pair) {
    m_item->m_bestList.append(pair);
}

/**
 * @brief SearchItemModel::removeItem
 */
void SearchItemModel::removeItem(QString path) {
    m_item->removeItem(path);
}

void SearchItemModel::clear() {
    this->beginResetModel();
    m_item->clear();
    this->endResetModel();
}

/**
 * @brief SearchItemModel::setBestAppIcon 设置最佳匹配应用的图标
 * @param str 图标名称或路径，应用安装时为图标名，未安装时为路径
 * @param is_installed 应用是否已安装
 */
void SearchItemModel::setBestAppIcon(const QString &str, const bool & is_installed) {
    if(is_installed) {
        m_item->m_bestAppIcon = QIcon::fromTheme(str);
    } else {
        m_item->m_bestAppIcon = QIcon(str);
    }
}

int SearchItemModel::length()
{
    return m_item->m_pathlist.length();
}
