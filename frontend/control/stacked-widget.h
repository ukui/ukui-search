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
#ifndef STACKEDWIDGET_H
#define STACKEDWIDGET_H

#include <QObject>
#include <QStackedWidget>
#include "search-line-edit.h"
#include "home-page.h"
#include "search-page.h"

namespace Zeeker {

enum class StackedPage {
    HomePage = 0,
    SearchPage
};

class StackedWidget : public QStackedWidget {
    Q_OBJECT
public:
    StackedWidget(QWidget *);
    ~StackedWidget();

    void setPage(const int &);
    int currentPage();
    void setPlugins(const QStringList&);
    void appendPlugin(const QString&);

Q_SIGNALS:
    void startSearch(const QString &);
    void stopSearch();
    void effectiveSearch();

private:
    void initWidgets();
    void initConnections();
    QString m_keyword;
    HomePage * m_homePage = nullptr;
    SearchPage * m_searchPage = nullptr;
//    StackedPage m_current_page;
};
}

#endif // STACKEDWIDGET_H
