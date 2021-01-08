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
#include "src/Interface/ukuimenuinterface.h"
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QSettings>
#include <QPropertyAnimation>
#include <QFileInfo>
#include <QPixmap>
#include <QKeyEvent>
#include "src/Style/style.h"
#include "src/MainWindow/mainviewwidget.h"
#include "src/Interface/ukuicontrolstyle.h"

class MainWindow : public UKuiMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    /**
     * @brief Load the main window
     */
    void loadMainWindow();
    void searchContent(QString searchcontent);

private:
    UkuiMenuInterface *m_ukuiMenuInterface=nullptr;

    QFrame *m_line=nullptr;//Vertical dividing line
    QFrame *m_frame=nullptr;
    MainViewWidget *m_mainViewWid=nullptr;
    QGSettings *gsetting;
    QGSettings *gsetting_position;
    QGSettings *bg_setting;
    QString bgPath;
    QString bgOption;
    QPixmap pixmap1;
    QPixmap *pixmap=nullptr;
    bool m_winFlag = false;

protected:
    void initUi();
    QPixmap * blurPixmap(QPixmap *pixmap);
    void paintEvent(QPaintEvent *event);
    void backgroundPic();
    void ways();
    /**
     * @brief Handle events clicking on the outside of the window
     */
    bool event(QEvent *event);
    void keyPressEvent(QKeyEvent *event);

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
};

#endif // MAINWINDOW_H
