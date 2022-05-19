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
#ifndef SEARCHRESULTMODEL_H
#define SEARCHRESULTMODEL_H
#include <QAbstractItemModel>
#include "search-worker-manager.h"

#define NUM_LIMIT_SHOWN_DEFAULT 5

namespace UkuiSearch {
class SearchResultCache;
class SearchResultModel : public QAbstractItemModel
{
    Q_OBJECT
public:

    enum DataRoles{
        IconRole = Qt::UserRole + 1,
        DisplayNameRole,
    };
//    Q_PROPERTY(bool expanded READ isExpanded WRITE setExpanded)
    SearchResultModel(QObject *parent = nullptr);
    ~SearchResultModel() = default;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role) const override;

//    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex())override;
    const SearchPluginIface::ResultInfo & getInfo(const QModelIndex&);
//    Q_INVOKABLE void setExpanded(const bool&);
//    Q_INVOKABLE const bool &isExpanded();
    QStringList getActions(const QModelIndex &);
    QString getKey(const QModelIndex &);
    void refresh();

public Q_SLOTS:
    void appendInfo(SearchPluginIface::ResultInfo resultInfo, QString pluginId);
    Q_INVOKABLE void startSearch(const QString &keyWord, const QStringList &pluginIds = QStringList());

Q_SIGNALS:
    void stopSearch();
    void itemListChanged(const int&);
    void sendBestListData(const QString &, const SearchPluginIface::ResultInfo&);

private:

    void initConnections();

    QMap<QString, SearchResultCache*> m_cache;
    SearchWorkerManager * m_searchWokerManager = nullptr;
    bool m_isExpanded = false;
    QTimer * m_timer;
};

class SearchResultCache
{
    friend class SearchResultModel;

public:
    SearchResultCache();
    SearchResultCache(QString pluginId);
    ~SearchResultCache() = default;

    int length();
    void clear();
    void setPluginId(const QString &pluginId);
    void setSectionDisplayName(const QString &sectionDisplayName);
    QString getSectionDisplayName();

private:
    QString m_pluginId;
    QString m_sectionDisplayName;
    QVector<SearchPluginIface::ResultInfo> m_resultInfoVector;
};
}

#endif // SEARCHRESULTMODEL_H
