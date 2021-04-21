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
#ifndef SEARCHITEMMODEL_H
#define SEARCHITEMMODEL_H

#include <QObject>
#include <QAbstractItemModel>
#include "search-item.h"

class SearchItem;

class SearchItemModel : public QAbstractItemModel
{
    friend class SearchItem;
    Q_OBJECT
public:
    explicit SearchItemModel(QObject *parent = nullptr);
    ~SearchItemModel();

    enum SearchInfo {
        Icon,
        Name
    };

    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex())override;
//    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setItem(SearchItem *);

    void appendItem(QString);
    void setList(QStringList);
    void setAppList(const QStringList&, const QStringList&);
    void appendBestItem(const QPair<int, QString>&);
    void removeItem(QString);
    void clear();
    void setBestAppIcon(const QString &, const bool &);

private :
    SearchItem * m_item = nullptr;
};

#endif // SEARCHITEMMODEL_H
