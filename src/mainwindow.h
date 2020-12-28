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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QPaintEvent>
#include <QPainter>
#include <QtMath>
#include <QEvent>
#include <QSpacerItem>
#include <QKeyEvent>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QSettings>
#include <QPropertyAnimation>
#include <QFileInfo>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <QGSettings/QGSettings>
#include "content-widget.h"
#include "input-box.h"
#include "index/index-generator.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    /**
     * @brief Load the main window
     */
    void searchContent(QString searchcontent);

private:
    bool nativeEvent(const QByteArray&, void *, long *);

    QFrame * m_line = nullptr;//Vertical dividing line
    QFrame * m_frame = nullptr;

    QFrame * m_titleFrame = nullptr;//标题栏
    QHBoxLayout * m_titleLyt = nullptr;
    QLabel * m_iconLabel = nullptr;
    QLabel * m_titleLabel = nullptr;
    QPushButton * m_menuBtn = nullptr;

    ContentWidget * m_contentFrame = nullptr;//内容栏

    UKuiSeachBarWidget * m_searchWidget = nullptr;//搜索栏
    UkuiSearchBarHLayout * m_searchLayout = nullptr;

    bool m_winFlag = false;

    QGSettings * m_transparency_gsettings = nullptr;
    double getTransparentData();

    QVector<int> m_types;
    QVector<QStringList> m_lists;
protected:
    void paintEvent(QPaintEvent *);
    void initUi();

public Q_SLOTS:
    /**
     * @brief Monitor screen resolution
     * @param rect: Screen resolution
     */
    void monitorResolutionChange(QRect rect);
    /**
     * @brief Monitor primary screen changes
     * @param screen: Primary screen
     */
    void primaryScreenChangedSlot(QScreen *screen);

    void bootOptionsFilter(QString opt);                              // 过滤终端命令
    void clearSearchResult(); //清空搜索结果
};

#endif // MAINWINDOW_H
