/*
 *
 * Copyright (C) 2021, KylinSoft Co., Ltd.
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
 * Authors: jixiaoxu <jixiaoxu@kylinos.cn>
 *
 */
#include "web-search-model.h"
#include <QDebug>

using namespace Zeeker;
WebSearchModel::WebSearchModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    m_item = new SearchResultItem;
}

QModelIndex WebSearchModel::index(int row, int column, const QModelIndex &parent) const
{
    if(row < 0 || row > m_item->m_result_info_list.length() - 1)
        return QModelIndex();
    return createIndex(row, column, m_item);
}

QModelIndex WebSearchModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int WebSearchModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    else
        return 1;
}

int WebSearchModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 1;
}

QVariant WebSearchModel::data(const QModelIndex &index, int role) const
{
    switch(role) {
    case Qt::DecorationRole: {
        return m_item->m_result_info_list.at(index.row()).icon;
    }
    case Qt::DisplayRole: {
        return m_item->m_result_info_list.at(index.row()).name;
    }
    default:
        return QVariant();
    }
    return QVariant();
}

void WebSearchModel::startSearch(const QString &keyword)
{
    this->beginResetModel();
    m_item->m_result_info_list.clear();
    SearchPluginIface::ResultInfo info;
    info.icon =  QIcon(":/res/icons/edit-find-symbolic.svg");
    info.name = keyword;
    m_item->m_result_info_list.append(info);
    this->endResetModel();
}
