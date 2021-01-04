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
#include <QApplication>
#include <syslog.h>
#include "src/Style/style.h"
#include <QPalette>
#include "kwindowsystem.h"

#define MODEL_SETTINGS      "org.ukui.SettingsDaemon.plugins.tablet-mode"
#define MODEL              "tablet-mode"

#define POSITION_SETTINGS  "org.ukui.SettingsDaemon.plugins.xrandr"
#define POSITION_KEY       "xrandr-rotations"

#define BACK_SETTINGS       "org.mate.background"
#define BACkNAME_KEY        "picture-filename"
#define BACkOPTION_KEY      "pictureOptions"

/**
 * @brief MainWindow 主界面
 * @param parent
 *
 * 慎用KWindowSystem::setShowingDesktop(!KWindowSystem::showingDesktop());
 * 可能造成窗口属性的混乱
 */
QT_BEGIN_NAMESPACE
extern void qt_blurImage(QPainter *p, QImage &blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0);
QT_END_NAMESPACE
MainWindow::MainWindow(QWidget *parent) :
    UKuiMainWindow(parent)
{
    const QByteArray back_id(BACK_SETTINGS);
    if(QGSettings::isSchemaInstalled(back_id)){
        bg_setting=new QGSettings(QString(BACK_SETTINGS).toLocal8Bit());
        bgPath=bg_setting->get(BACkNAME_KEY).toString();
        bgOption = bg_setting->get(BACkOPTION_KEY).toString();
        ways();//壁纸显示方式
    }

        //开机第一次检测模式执行对应的模式
    const QByteArray panelmodel_id(MODEL_SETTINGS);
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
    const QByteArray position(POSITION_SETTINGS);
    //开机第一次检测模式执行对应的任务栏
    if(QGSettings::isSchemaInstalled(position)){
        gsetting_position = new QGSettings(position);
    }
    connect(gsetting_position, &QGSettings::changed,this, [=](const QString &key) {
        QString direction=gsetting_position->get("xrandr-rotations").toString();
        if(direction=="left" || direction=="right")
        {
            this->setFixedSize(QApplication::primaryScreen()->geometry().height(),QApplication::primaryScreen()->geometry().width());
        }
        else{
            this->setFixedSize(QApplication::primaryScreen()->geometry().width(),QApplication::primaryScreen()->geometry().height());
        }
    });
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

void MainWindow::ways()
{
    pixmap1=QPixmap(bgPath);
    if (bgOption == ""||bgOption == NULL)
        bgOption = "stretched";
    if (bgOption== "centered"||bgOption=="scaled"||bgOption=="wallpaper") //居中
    {
        pixmap->load(bgPath);
    }
    else
    {
        pixmap1=pixmap1.scaled(this->size());
        pixmap=&pixmap1;
    }
    pixmap=blurPixmap(pixmap);
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
//    qDebug()<<event->type();
    switch (event->type()){
    case QEvent::ActivationChange:
        if(QApplication::activeWindow() != this){
            qApp->exit(0);
        }
        break;
    case QEvent::MouseButtonRelease:
        qApp->exit(0);
        break;
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

QPixmap * MainWindow::blurPixmap(QPixmap *pixmap)
{
    QPainter painter(pixmap);
    QImage srcImg=pixmap->toImage();
    qt_blurImage(&painter,srcImg,150,false,false);//top 27000
    painter.end();
    return pixmap;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    backgroundPic();//bgPath,picrect
    //在每个屏幕上绘制背景
    return QWidget::paintEvent(event);
}
void MainWindow::backgroundPic()//const QString &bgPath,QRect rect
{
    QPainter painter(this);
    if (bgOption == ""||bgOption == NULL)
        bgOption = "stretched";
    if (bgOption== "centered") //居中
    {
        painter.drawPixmap(this->width()/2-pixmap->width()/2,
                           this->height()/2-pixmap->height()/2,
                           *pixmap);
    }
    else if (bgOption=="stretched"||bgOption=="scaled") //拉伸
    {
        painter.drawPixmap(this->rect(),*pixmap);
    }
    else if (bgOption=="wallpaper") //平铺
    {
        int drawedWidth=0;
        int drawedHeight=0;
        while(1)
        {
            drawedWidth=0;
            while(1)
            {
                painter.drawPixmap(drawedWidth,drawedHeight,*pixmap);
                drawedWidth += pixmap->width();
                if(drawedWidth>=this->width())
                    break;
            }
            drawedHeight += pixmap->height();
            if(drawedHeight >= this->height())
                break;
        }
    }
    else
    {
        painter.drawPixmap(this->rect(),*pixmap);
    }
}

void MainWindow::searchContent(QString searchcontent){
    m_mainViewWid->searchContent(searchcontent);
}
