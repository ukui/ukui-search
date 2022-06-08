//
// Created by hxf on 22-5-24.
//

#include "search-plugin-model.h"
#include "search-plugin-manager.h"

using namespace UkuiSearch;

SearchPluginModel::SearchPluginModel(QObject *parent) : QAbstractListModel(parent)
{
    m_searchPluginManager = SearchPluginManager::getInstance();
    for (const QString &item: m_searchPluginManager->getPluginIds()) {
        m_plugins.push_back(item);
    }

    m_roleNames[SearchPluginModel::IdRole] = "id";
    m_roleNames[SearchPluginModel::NameRole] = "name";
    m_roleNames[SearchPluginModel::IconRole] = "icon";
    m_roleNames[SearchPluginModel::IconNameRole] = "iconName";
    m_roleNames[SearchPluginModel::DescriptionRole] = "description";
    m_roleNames[SearchPluginModel::DetailsDescRole] = "details";
}

int SearchPluginModel::rowCount(const QModelIndex &parent) const
{
    return m_plugins.size();
}

QVariant SearchPluginModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if (row < 0 || row >= m_plugins.size()) {
        return {};
    }

    QString pluginId = m_plugins[row];
    switch (role) {
        case SearchPluginModel::IdRole:
            return pluginId;

        case SearchPluginModel::NameRole:
            return m_searchPluginManager->getPlugin(pluginId)->getPluginName();

        case SearchPluginModel::IconRole:
            return m_searchPluginManager->getPlugin(pluginId)->icon();

        case SearchPluginModel::IconNameRole:
            return m_searchPluginManager->getPlugin(pluginId)->icon().name();

        case SearchPluginModel::DescriptionRole:
            return m_searchPluginManager->getPlugin(pluginId)->description();

        case SearchPluginModel::DetailsDescRole:
            return m_searchPluginManager->getPlugin(pluginId)->detailPageDesc();

        default:
            return {};
    }
}

QHash<int, QByteArray> SearchPluginModel::roleNames() const
{
    return m_roleNames;
}
