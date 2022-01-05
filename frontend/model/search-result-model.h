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
#include "search-result-manager.h"

#define NUM_LIMIT_SHOWN_DEFAULT 5

namespace Zeeker {

class SearchResultItem : public QObject {
    friend class SearchResultModel;
    friend class BestListModel;
    friend class WebSearchModel;
    Q_OBJECT
public:
    explicit SearchResultItem(QObject *parent = nullptr);
    ~SearchResultItem() = default;
private:
    //此插件所有搜索结果<具体信息>
   QVector<SearchPluginIface::ResultInfo> m_result_info_list;
};
class SearchResultModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    SearchResultModel(const QString &plugin_id);
    ~SearchResultModel() = default;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
//    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex())override;
    const SearchPluginIface::ResultInfo & getInfo(const QModelIndex&);
    void setExpanded(const bool&);
    const bool &isExpanded();
    QStringList getActions(const QModelIndex &);
    QString getKey(const QModelIndex &);
    void refresh();

public Q_SLOTS:
    void appendInfo(const SearchPluginIface::ResultInfo &);
    void startSearch(const QString &);

Q_SIGNALS:
    void stopSearch();
    void itemListChanged(const int&);

private:
    void initConnections();
    SearchResultItem * m_item = nullptr;
    QString m_plugin_id;
    SearchResultManager * m_search_manager = nullptr;
    bool m_isExpanded = false;
    QTimer * m_timer;
};
}

#endif // SEARCHRESULTMODEL_H
