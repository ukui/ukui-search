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

SearchResultModel::SearchResultModel(QObject *parent) : QAbstractItemModel(parent)
{
    m_searchWokerManager = new SearchWorkerManager(this);
    for(QString id : m_searchWokerManager->getPluginIds()) {
        m_cache.insert(id, new SearchResultCache(id));
    }
    m_timer = new QTimer(this);
    m_timer->setInterval(500);
    connect(this, &SearchResultModel::stopSearch, m_searchWokerManager, &SearchWorkerManager::stopSearch);
    connect(m_searchWokerManager, &SearchWorkerManager::gotResultInfo, this, &SearchResultModel::appendInfo);
}

QModelIndex SearchResultModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return createIndex(row, column);
}

QModelIndex SearchResultModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

int SearchResultModel::rowCount(const QModelIndex &index) const
{
//    return index.isValid() ? 0 : (m_isExpanded ? m_item->m_resultInfoVector.length() : NUM_LIMIT_SHOWN_DEFAULT);
    return 0;
}

int SearchResultModel::columnCount(const QModelIndex &index) const
{
    return index.isValid() ? 0 : 1;
}

QHash<int, QByteArray> SearchResultModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IconRole] = "icon";
    roles[DisplayNameRole] = "displayName";
    return roles;
}

QVariant SearchResultModel::data(const QModelIndex &index, int role) const
{
    switch(role) {
//    case IconRole: {
//        return m_item ? QVariant() : m_item->m_resultInfoVector.at(index.row()).icon;
//    }
//    case DisplayNameRole: {
//        qDebug() << m_item->m_resultInfoVector.at(index.row()).name;
//        return m_item ? QVariant() : m_item->m_resultInfoVector.at(index.row()).name;
//    }
    default:
        return QVariant();
    }
    return QVariant();
}

void SearchResultModel::appendInfo( SearchPluginIface::ResultInfo resultInfo, QString pluginId)//TODO 代码逻辑可尝试梳理优化
{
//    if (m_item->m_resultInfoVector.length() > 5 //搜索结果大于5个并且搜索结果处于收起状态时只存储数据无需刷新UI
//            and !m_isExpanded) {
//        m_item->m_resultInfoVector.append(resultInfo);
//        return;
//    }
//    if (m_item->m_resultInfoVector.length() > 50
//            and m_isExpanded) {//搜索结果大于50个并且搜索结果处于展开状态时只存储数据并启动定时，500ms刷新一次UI
//        m_item->m_resultInfoVector.append(resultInfo);
//        if (!m_timer->isActive()) {
//            m_timer->start();
//        }
//        return;
//    }

    this->beginResetModel();
//    m_cache.value(pluginId).m_resultInfoVector.append(resultInfo);
    this->endResetModel();
}

void SearchResultModel::startSearch(const QString &keyword, const QStringList &pluginIds)
{
    m_searchWokerManager->startSearch(keyword);
}

void SearchResultModel::initConnections()
{

//    connect(m_timer, &QTimer::timeout, [ = ] () {//500ms刷新一次UI,防止搜索结果数据量过大导致的UI卡顿
//        Q_EMIT this->itemListChanged(m_item->m_resultInfoVector.length());
//        m_timer->stop();
//    });
}

const SearchPluginIface::ResultInfo &SearchResultModel::getInfo(const QModelIndex &index)
{
    return SearchPluginIface::ResultInfo{};
}

//void SearchResultModel::setExpanded(const bool &is_expanded)
//{
//    this->beginResetModel();
//    m_isExpanded = is_expanded;
//    this->endResetModel();
//    Q_EMIT this->itemListChanged(m_item->m_resultInfoVector.length());
//}

//const bool &SearchResultModel::isExpanded()
//{
//    return m_isExpanded;
//}

/**
 * @brief SearchResultModel::getActions 获取操作列表
 * @param index
 * @return
 */
QStringList SearchResultModel::getActions(const QModelIndex &index)
{
//    if (m_cache.length() > index.row() && index.row() >= 0)
//        return m_item->m_result_info_list.at(index.row()).actionList;
    return QStringList();
}

QString SearchResultModel::getKey(const QModelIndex &index)
{
//    if (m_cache.length() > index.row() && index.row() >= 0)
//        return m_item->m_result_info_list.at(index.row()).key;
        return QString();
}

//void SearchResultModel::setPluginId(QString pluginId)
//{
//    m_pluginId = pluginId;
//    qDebug() << pluginId << "11111";
//    m_searchWokerManager = new SearchWorkerManager(pluginId);
//    initConnections();
//}

//QString SearchResultModel::getPluginId()
//{
//    return m_pluginId;
//}

void SearchResultModel::refresh()
{
    this->beginResetModel();
    this->endResetModel();
}

SearchResultCache::SearchResultCache()
{
}

SearchResultCache::SearchResultCache(QString pluginId) : m_pluginId(pluginId)
{
}

int SearchResultCache::length()
{
    return m_resultInfoVector.length();
}

void SearchResultCache::clear()
{
    m_resultInfoVector.clear();
}

void SearchResultCache::setPluginId(const QString &pluginId)
{
    m_pluginId = pluginId;
}

void SearchResultCache::setSectionDisplayName(const QString &sectionDisplayName)
{
    m_sectionDisplayName = sectionDisplayName;
}
