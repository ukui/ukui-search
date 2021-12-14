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
#include "search-result-model.h"
using namespace UkuiSearch;

SearchResultModel::SearchResultModel(const QString &plugin_id)
{
    m_item = new SearchResultItem;
    m_plugin_id = plugin_id;
    m_search_manager = new SearchResultManager(plugin_id);
    initConnections();
}

QModelIndex SearchResultModel::index(int row, int column, const QModelIndex &parent) const
{
    if(row < 0 || row > m_item->m_result_info_list.length() - 1)
        return QModelIndex();
//    QVector<SearchPluginIface::ResultInfo> * m_info = &m_result_info_list;
    return createIndex(row, column, m_item);
}

QModelIndex SearchResultModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

int SearchResultModel::rowCount(const QModelIndex &index) const
{
    return index.isValid() ? 0 : (m_isExpanded ? m_item->m_result_info_list.length() : NUM_LIMIT_SHOWN_DEFAULT);
}

int SearchResultModel::columnCount(const QModelIndex &index) const
{
    return index.isValid() ? 0 : 1;
}

QVariant SearchResultModel::data(const QModelIndex &index, int role) const
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

//bool SearchResultModel::insertRows(int row, int count, const QModelIndex &parent)
//{
//    this->beginInsertRows(parent, row, count);
//    this->endInsertRows();
//    return true;
//}

void SearchResultModel::appendInfo(const SearchPluginIface::ResultInfo &info)
{
    this->beginResetModel();
    //qDebug()<<"Got a result. name ="<<info.name;
    m_item->m_result_info_list.append(info);
    this->endResetModel();
    Q_EMIT this->itemListChanged(m_item->m_result_info_list.length());
    if (m_plugin_id != "Web Page") {
        Q_EMIT this->sendBestListData(m_plugin_id, m_item->m_result_info_list.at(0));
    }
}

void SearchResultModel::startSearch(const QString &keyword)
{
    if (!m_item->m_result_info_list.isEmpty()) {
        this->beginResetModel();
        m_item->m_result_info_list.clear();
        this->endResetModel();
        Q_EMIT this->itemListChanged(m_item->m_result_info_list.length());
    }
    m_search_manager->startSearch(keyword);
}

void SearchResultModel::initConnections()
{
    connect(this, &SearchResultModel::stopSearch, m_search_manager, &SearchResultManager::stopSearch);
    connect(m_search_manager, &SearchResultManager::gotResultInfo, this, &SearchResultModel::appendInfo);
}

const SearchPluginIface::ResultInfo &SearchResultModel::getInfo(const QModelIndex &index)
{
    return m_item->m_result_info_list.at(index.row());
}

void SearchResultModel::setExpanded(const bool &is_expanded)
{
    this->beginResetModel();
    m_isExpanded = is_expanded;
    this->endResetModel();
    Q_EMIT this->itemListChanged(m_item->m_result_info_list.length());
}

const bool &SearchResultModel::isExpanded()
{
    return m_isExpanded;
}

/**
 * @brief SearchResultModel::getActions 获取操作列表
 * @param index
 * @return
 */
QStringList SearchResultModel::getActions(const QModelIndex &index)
{
    if (m_item->m_result_info_list.length() > index.row() && index.row() >= 0)
//        return m_item->m_result_info_list.at(index.row()).actionList;
    return QStringList();
}

QString SearchResultModel::getKey(const QModelIndex &index)
{
    if (m_item->m_result_info_list.length() > index.row() && index.row() >= 0)
//        return m_item->m_result_info_list.at(index.row()).key;
    return NULL;
}

SearchResultItem::SearchResultItem(QObject *parent) : QObject(parent)
{

}
