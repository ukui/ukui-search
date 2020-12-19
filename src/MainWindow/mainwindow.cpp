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

#define MODEL_SETTINGS      "org.ukui.SettingsDaemon.plugins.tablet-mode"
#define MODEL              "tablet-mode"

/**
 * @brief MainWindow 主界面
 * @param parent
 *
 * 慎用KWindowSystem::setShowingDesktop(!KWindowSystem::showingDesktop());
 * 可能造成窗口属性的混乱
 */
MainWindow::MainWindow(QWidget *parent) :
    UKuiMainWindow(parent)
{
    const QByteArray panelmodel_id(MODEL_SETTINGS);
    //开机第一次检测模式执行对应的任务栏
    if(QGSettings::isSchemaInstalled(panelmodel_id)){
        gsetting = new QGSettings(panelmodel_id);
    }
    m_ukuiMenuInterface=new UkuiMenuInterface;
    UkuiMenuInterface::appInfoVector=m_ukuiMenuInterface->createAppInfoVector();
    UkuiMenuInterface::alphabeticVector=m_ukuiMenuInterface->getAlphabeticClassification();
    UkuiMenuInterface::functionalVector=m_ukuiMenuInterface->getFunctionalClassification();
    UkuiMenuInterface::allAppVector=m_ukuiMenuInterface->getAllApp();
    Style::initWidStyle();
    initUi();
}

MainWindow::~MainWindow()
{
    delete m_ukuiMenuInterface;
}

/**
 * @brief initUi
 * 设置本窗口的大小 this->setFixedSize(Style::minw,Style::minh);
 */
void MainWindow::initUi()
{
    this->setFixedSize(Style::minw,Style::minh);
    this->setAttribute(Qt::WA_AcceptTouchEvents);
    this->setWindowFlags(Qt::Popup);
    m_frame=new QFrame;
    m_mainViewWid=new MainViewWidget(this);

    this->setCentralWidget(m_frame);
    QHBoxLayout *mainlayout=new QHBoxLayout;
    mainlayout->setContentsMargins(0,80,0,0);
    m_frame->setLayout(mainlayout);

    mainlayout->addWidget(m_mainViewWid);

    connect(QApplication::primaryScreen(),&QScreen::geometryChanged,
            this,&MainWindow::monitorResolutionChange);
    connect(qApp,&QApplication::primaryScreenChanged,this,
            &MainWindow::primaryScreenChangedSlot);
}

/**
 * @brief bootOptionsFilter 过滤终端命令
 * @param opt
 */
void MainWindow::bootOptionsFilter(QString opt)
{

    if (opt == "-s" || opt == "-show") {
//        qDebug() << "第一次用命令进入";
    }
}

/**
 * 鼠标点击窗口外部事件
 */
bool MainWindow::event ( QEvent * event )
{
    qDebug()<<event->type();
    const QByteArray panelmodel_id(MODEL_SETTINGS);
    if(QGSettings::isSchemaInstalled(panelmodel_id))
    if(event->type()==QEvent::MouseButtonPress){
        if(QGSettings::isSchemaInstalled(panelmodel_id))
        {
            if(!gsetting->get(MODEL).toBool()){
                this->close();
                return true;
            }
        }
    }
    if(event->type()==QEvent::GestureOverride){
        return false;
    }
    if(event->type()==QEvent::MouseButtonRelease){
        this->close();
        return true;
    }
    return QWidget::event(event);
}

/**
 * @brief loadMainWindow 加载主界面的函数
 * 不删除的原因是在单例和main函数里面需要用
 */
void MainWindow::loadMainWindow()
{
    QDesktopWidget* m = QApplication::desktop();
    QRect desk_rect = m->screenGeometry(m->screenNumber(QCursor::pos()));
    int desk_x = desk_rect.width();
    int desk_y = desk_rect.height();
    int x = this->width();
    int y = this->height();
//    this->move(desk_x / 2 - x / 2 + desk_rect.left(), desk_y / 2 - y / 2 + desk_rect.top());
    m_mainViewWid->loadMinMainView();
}

/**
 * @brief monitorResolutionChange  监听屏幕改变
 * @param rect
 */
void MainWindow::monitorResolutionChange(QRect rect)
{
    Q_UNUSED(rect);
}

/**
 * @brief primaryScreenChangedSlot 监听分辨率改变
 * @param screen
 */
void MainWindow::primaryScreenChangedSlot(QScreen *screen)
{
    Q_UNUSED(screen);

}

void MainWindow::searchContent(QString searchcontent){
    m_mainViewWid->searchContent(searchcontent);
}
