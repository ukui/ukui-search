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
#ifndef HOMEPAGEITEM_H
#define HOMEPAGEITEM_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "file-utils.h"
#include "search-list-view.h"

class HomePageItem : public QWidget {
    Q_OBJECT
public:
    explicit HomePageItem(QWidget *, const int&, const QString&);
    ~HomePageItem();

    enum ItemType { //homepage中item的类型，包括常用应用、最近打开、快捷打开
        Quick,
        Recent,
        Common
    };

protected:
    bool eventFilter(QObject *, QEvent *);
    void paintEvent(QPaintEvent *);

private:
    void setupUi(const int&, const QString&);
    void onItemClicked();

    QWidget * m_widget = nullptr;
    QHBoxLayout * m_hlayout = nullptr;
    QVBoxLayout * m_vlayout = nullptr;
    QLabel * m_iconlabel = nullptr;
    QLabel * m_namelabel = nullptr;
    double m_transparency = 0;
    QString m_path;
    int m_type = 0;
};

#endif // HOMEPAGEITEM_H
