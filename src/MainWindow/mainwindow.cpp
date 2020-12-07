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

#include "mainwindow.h"
#include <QHBoxLayout>
#include <QDebug>
#include <QDesktopWidget>
#include <QtSingleApplication>
#include <QtX11Extras/QX11Info>
#include <syslog.h>
#include "src/Style/style.h"
#include <QPalette>
#include "kwindowsystem.h"

/*主界面
 * 函数分析：
 * initUi:初始化ui
 * setFrameStyle：设置样式
 * bootOptionsFilter：过滤终端命令
 * event：鼠标点击窗口外部事件
 * loadMainWindow：加载主界面
 *
 * primaryScreenChangedSlot：监听屏幕改变
 * monitorResolutionChange： 监听分辨率改变
 *
*/
MainWindow::MainWindow(QWidget *parent) :
    UKuiMainWindow(parent)
{
    m_ukuiMenuInterface=new UkuiMenuInterface;
    UkuiMenuInterface::appInfoVector=m_ukuiMenuInterface->createAppInfoVector();
    UkuiMenuInterface::alphabeticVector=m_ukuiMenuInterface->getAlphabeticClassification();
    UkuiMenuInterface::functionalVector=m_ukuiMenuInterface->getFunctionalClassification();
    UkuiMenuInterface::allAppVector=m_ukuiMenuInterface->getAllApp();
    Style::initWidStyle();
    initUi();

//    KWindowSystem::setShowingDesktop(!KWindowSystem::showingDesktop());
}

MainWindow::~MainWindow()
{
//    KWindowSystem::setShowingDesktop(!KWindowSystem::showingDesktop());
    delete m_ukuiMenuInterface;
}

void MainWindow::initUi()
{
    this->setFixedWidth(Style::minw);//可设置本窗口的大小，调整见style文件参数]
    this->setContentsMargins(0,0,0,0);

    m_frame=new QFrame;
    m_mainViewWid=new MainViewWidget;

    this->setCentralWidget(m_frame);
    QVBoxLayout *mainlayout=new QVBoxLayout;
    mainlayout->setContentsMargins(0,0,0,0);
    mainlayout->setSpacing(0);
    m_frame->setLayout(mainlayout);

    mainlayout->addWidget(m_mainViewWid);

    connect(QApplication::primaryScreen(),&QScreen::geometryChanged,
            this,&MainWindow::monitorResolutionChange);
    connect(qApp,&QApplication::primaryScreenChanged,this,
            &MainWindow::primaryScreenChangedSlot);
}

/* 过滤终端命令 */
void MainWindow::bootOptionsFilter(QString opt)
{

    if (opt == "-s" || opt == "-show") {
//        qDebug() << "哈哈哈哈，第一次用命令进入这";

    }
}

/**
 * 鼠标点击窗口外部事件
 */
bool MainWindow::event ( QEvent * event )
{
    if (event->type() == QEvent::ActivationChange)
    {
        if(QApplication::activeWindow() != this)
        {
            this->close();
        }
    }
    return QWidget::event(event);
}

void MainWindow::loadMainWindow()
{
    QDesktopWidget* m = QApplication::desktop();
    QRect desk_rect = m->screenGeometry(m->screenNumber(QCursor::pos()));
    int desk_x = desk_rect.width();
    int desk_y = desk_rect.height();
    int x = this->width();
    int y = this->height();
    this->move(desk_x / 2 - x / 2 + desk_rect.left(), desk_y / 2 - y / 2 + desk_rect.top());
    m_mainViewWid->loadMinMainView();
}

void MainWindow::monitorResolutionChange(QRect rect)
{
    Q_UNUSED(rect);
}

void MainWindow::primaryScreenChangedSlot(QScreen *screen)
{
    Q_UNUSED(screen);

}

void MainWindow::searchContent(QString searchcontent){
    m_mainViewWid->searchContent(searchcontent);
}
