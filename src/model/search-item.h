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
#ifndef SEARCHITEM_H
#define SEARCHITEM_H

#include <QObject>
#include <QPixmap>
#include "search-item-model.h"
#include "file-utils.h"

namespace Zeeker {
class SearchItem : public QObject {
    friend class SearchItemModel;
    friend class SearchListView;
    Q_OBJECT
public:
    explicit SearchItem(QObject *parent = nullptr);
    ~SearchItem();

    enum SearchType {
        Best,
        Apps,
        Settings,
        Dirs,
        Files,
        Contents,
        Web
    };

    void setSearchList(const int&, const QStringList&);
    void removeItem(QString);
    int getCurrentSize();
    void clear();

private:
//    SearchItem * m_parent = nullptr;
//    QVector<SearchItem*> * m_children = nullptr;

    int m_searchtype = 0;
    QStringList m_pathlist;
    QStringList m_app_pathlist;
    QStringList m_app_iconlist;
    QList<QPair<int, QString>> m_bestList;

    QIcon m_bestAppIcon = QIcon::fromTheme("unknown",QIcon(":res/icons/unknown.png")); //由于未安装应用与已安装应用获取图标方式不一致，故引入此变量直接存储最佳匹配应用图标

    QIcon getIcon(int);
    QString getName(int);
    QIcon getBestIcon(const int &);
    QString getBestName(const int &);

Q_SIGNALS:

};
}

#endif // SEARCHITEM_H
