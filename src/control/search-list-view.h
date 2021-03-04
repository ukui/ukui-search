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
#ifndef SEARCHLISTVIEW_H
#define SEARCHLISTVIEW_H

#include <QObject>
#include <QTreeView>
#include "model/search-item-model.h"
#include "model/search-item.h"
#include "highlight-item-delegate.h"

class SearchListView : public QTreeView
{
    Q_OBJECT
public:
    explicit SearchListView(QWidget *, const QStringList&, const int&);
    ~SearchListView();

    enum ResType { //搜索结果可能出现的类型：应用、文件、设置、文件夹
        Best,
        App,
        Setting,
        Dir,
        File,
        Content,
        Web
    };

    int getCurrentType();
    static int getResType(const QString&);

    bool is_current_list = false;
    int rowheight = 0;

    void appendItem(QString);
    void setList(QStringList);
    void setAppList(const QStringList&, const QStringList&);
    void appendBestItem(const QPair<int, QString>&);
    void removeItem(QString);
    void clear();
    void setKeyword(QString);
    int getType();
    int getLength();
    bool isHidden = false;
private:
    SearchItemModel * m_model = nullptr;
    SearchItem * m_item = nullptr;
    bool m_isSelected = false;

    HighlightItemDelegate * m_styleDelegate = nullptr;

    int m_type;

Q_SIGNALS:
    void currentRowChanged(const int&, const QString&);

public Q_SLOTS:
    void clearSelection();
};

#endif // SEARCHLISTVIEW_H
