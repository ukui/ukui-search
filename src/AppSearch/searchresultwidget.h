/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */

#ifndef SEARCHRESULTWIDGET_H
#define SEARCHRESULTWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QDesktopWidget>
#include <QLabel>
#include <QToolButton>
#include <QScrollBar>
#include <QPushButton>
#include <gio/gdesktopappinfo.h>
#include "src/Interface/ukuimenuinterface.h"
#include "src/UtilityFunction/listview.h"
#include "src/UtilityFunction/itemdelegate.h"
#include "../Interface/morebutton.h"
#include <QVBoxLayout>
#include "searchappthread.h"
#include <QPainter>

class SearchResultWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SearchResultWidget(QWidget *parent = nullptr);
    ~SearchResultWidget();
    /**
     * @brief Update application list
     */
    void updateAppListView(QVector<QStringList> arg);
private:
    UkuiMenuInterface* m_ukuiMenuInterface=nullptr;
    ListView* m_listView=nullptr;
    QVector<QStringList> m_data;
    SearchAppThread *m_searchAppThread=nullptr;
    void paintEvent(QPaintEvent *e);
    bool eventFilter(QObject *watched, QEvent *event);

    QVBoxLayout *m_listLayout;
    QLabel * applabel;
    MoreButton *m_Button;

protected:
    /**
     * @brief Initializes UI
     */
    void initUi();

private Q_SLOTS:
    /**
     * @brief Open the application
     * @param arg: The desktop file information that the application contains
     */
    void execApplication(QStringList arg);
    void appTextRefresh(QString);
    void recvSearchResult(QVector<QStringList> arg);
Q_SIGNALS:
    void changeAppNum(int num);
    void sendSearchKeyword(QString arg);
};

#endif // SEARCHRESULTWIDGET_H
