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
 * Authors: jixiaoxu <jixiaoxu@kylinos.cn>
 *
 */
#include "best-list-model.h"
#include "search-plugin-manager.h"
using namespace Zeeker;

BestListModel::BestListModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    m_item = new SearchResultItem;
    initConnections();
}

QModelIndex BestListModel::index(int row, int column, const QModelIndex &parent) const
{
    if(row < 0 || row > m_item->m_result_info_list.length() - 1)
        return QModelIndex();
    return createIndex(row, column, m_item);
}

QModelIndex BestListModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int BestListModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : (m_isExpanded ? m_item->m_result_info_list.length() : NUM_LIMIT_SHOWN_DEFAULT);
}

int BestListModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 1;
}

QVariant BestListModel::data(const QModelIndex &index, int role) const
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

const SearchPluginIface::ResultInfo &BestListModel::getInfo(const QModelIndex &index)
{
    return m_item->m_result_info_list.at(index.row());
}

const QString &BestListModel::getPluginInfo(const QModelIndex &index)
{
    return m_plugin_id_list.at(index.row());
}

void BestListModel::setExpanded(const bool &is_expanded)
{
    this->beginResetModel();
    m_isExpanded = is_expanded;
    this->endResetModel();
    Q_EMIT this->itemListChanged(m_item->m_result_info_list.length());
}

const bool &BestListModel::isExpanded()
{
    return m_isExpanded;
}

QStringList BestListModel::getActions(const QModelIndex &index)
{
//    if (m_item->m_result_info_list.length() > index.row() && index.row() >= 0)
//        return m_item->m_result_info_list.at(index.row()).actionList;
    return QStringList();
}

QString BestListModel::getKey(const QModelIndex &index)
{
    if (m_item->m_result_info_list.length() > index.row() && index.row() >= 0)
        return m_item->m_result_info_list.at(index.row()).actionKey;
    return NULL;
}

void BestListModel::appendInfo(const QString &pluginId, const SearchPluginIface::ResultInfo &info)
{
    if (pluginId == "File Search" && m_fileActionKey_tmp != info.actionKey) {//临时保存文件搜索结果的actionKey
        m_fileActionKey_tmp = info.actionKey;
    }
    if (pluginId == "File Content Search" && info.actionKey == m_fileActionKey_tmp) {//文本搜索判断是否和保存的actionKey相同
        return;
    }
    if (m_plugin_id_list.contains(pluginId)) {
        if (info.name == m_item->m_result_info_list.at(m_plugin_id_list.lastIndexOf(pluginId)).name) {
            return;
        }
        qDebug()<<"plugin ID:"<<pluginId<<"Repalce result. name ="<<info.name;
        this->beginResetModel();
        m_item->m_result_info_list.replace(m_plugin_id_list.lastIndexOf(pluginId), info);
        this->endResetModel();
        return;
    }
    this->beginResetModel();
    qDebug()<<"plugin ID:"<<pluginId<<"Got a result. name ="<<info.name;
    m_plugin_id_list.append(pluginId);
    m_item->m_result_info_list.append(info);
    QVector<SearchPluginIface::ResultInfo> result_info_list_tmp;
    QVector<QString> plugin_id_list_tmp;
    QStringList plugin_order = SearchPluginManager::getInstance()->getPluginIds();
    Q_FOREACH (QString plugin, plugin_order) {
        if (m_plugin_id_list.contains(plugin)) {
            result_info_list_tmp.append(m_item->m_result_info_list.at(m_plugin_id_list.lastIndexOf(plugin)));
            plugin_id_list_tmp.append(plugin);
        }
    }
    m_item->m_result_info_list.clear();
    m_item->m_result_info_list.swap(result_info_list_tmp);
    m_plugin_id_list.clear();
    m_plugin_id_list.swap(plugin_id_list_tmp);
    this->endResetModel();
    Q_EMIT this->itemListChanged(m_item->m_result_info_list.length());

}

void BestListModel::startSearch(const QString &keyword)
{
    if (!m_item->m_result_info_list.isEmpty()) {
        this->beginResetModel();
        m_plugin_id_list.clear();
        m_item->m_result_info_list.clear();
        this->endResetModel();
        Q_EMIT this->itemListChanged(m_item->m_result_info_list.length());
    }
}

void BestListModel::initConnections()
{

}
