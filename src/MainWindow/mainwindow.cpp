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
#include "src/XEventMonitor/xeventmonitor.h"
#include "src/Style/style.h"

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
    QMainWindow(parent)
{
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
    XEventMonitor::instance()->quit();
    delete m_ukuiMenuInterface;
}

void MainWindow::initUi()
{
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setAutoFillBackground(false);
    this->setFocusPolicy(Qt::StrongFocus);

    this->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    this->setMinimumSize(Style::minw,Style::minh);//可设置本窗口的大小，调整见style文件参数
    this->setContentsMargins(0,0,0,0);

    m_frame=new QFrame;
    m_mainViewWid=new MainViewWidget;

    this->setCentralWidget(m_frame);
    QVBoxLayout *mainlayout=new QVBoxLayout;
    mainlayout->setContentsMargins(0,0,0,0);
    mainlayout->setSpacing(0);
    m_frame->setLayout(mainlayout);

    mainlayout->addWidget(m_mainViewWid);
    m_line=new QFrame;
    m_line->setFrameShape(QFrame::VLine);
    m_line->setFixedSize(1,this->height());
    mainlayout->addWidget(m_line);
    char linestyle[100];
    sprintf(linestyle, "background-color:%s;",LineBackground);
    m_line->setStyleSheet(linestyle);

    connect(QApplication::primaryScreen(),&QScreen::geometryChanged,
            this,&MainWindow::monitorResolutionChange);
    connect(qApp,&QApplication::primaryScreenChanged,this,
            &MainWindow::primaryScreenChangedSlot);

    XEventMonitor::instance()->start();
    connect(XEventMonitor::instance(), SIGNAL(keyRelease(QString)),
            this,SLOT(XkbEventsRelease(QString)));
    connect(XEventMonitor::instance(), SIGNAL(keyPress(QString)),
            this,SLOT(XkbEventsPress(QString)));
}

/* 过滤终端命令 */
void MainWindow::bootOptionsFilter(QString opt)
{

    if (opt == "-s" || opt == "-show") {
        qDebug() << "哈哈哈哈，第一次用命令进入这";

    }
}

//void centerToScreen(QWidget* widget) {
//    if (!widget)
//      return;
//    QDesktopWidget* m = QApplication::desktop();
//    QRect desk_rect = m->screenGeometry(m->screenNumber(QCursor::pos()));
//    int desk_x = desk_rect.width();
//    int desk_y = desk_rect.height();
//    int x = widget->width();
//    int y = widget->height();
//    widget->move(desk_x / 2 - x / 2 + desk_rect.left(), desk_y / 2 - y / 2 + desk_rect.top());
//}

/**
 * 鼠标点击窗口外部事件
 */
bool MainWindow::event ( QEvent * event )
{
    if (event->type() == QEvent::ActivationChange)
    {
        if(QApplication::activeWindow() != this)
        {
            //this->hide();
            this->deleteLater();
            m_mainViewWid->widgetMakeZero();
        }
    }
    return QWidget::event(event);
}

/*XkbEventsPress 和XkbEventsRelease 为设置快捷键的函数
 * 采用先注释，验证结束再删除的方式
 * 经过两轮（两周）测试验证无问题后删除这两个函数及调用
*/
void MainWindow::XkbEventsPress(const QString &keycode)
{
//    QString KeyName;
//    if (keycode.length() >= 8){
//        KeyName = keycode.left(8);
//    }
//    if(KeyName.compare("Super_L+")==0){
//        m_winFlag = true;
//    }
//    if(m_winFlag && keycode == "Super_L"){
//        m_winFlag = false;
//        return;
//    }

}

void MainWindow::XkbEventsRelease(const QString &keycode)
{
//    QString KeyName;
//    static bool winFlag=false;
//    if (keycode.length() >= 8){
//        KeyName = keycode.left(8);
//    }
//    if(KeyName.compare("Super_L+")==0){
//        winFlag = true;
//    }
//    if(winFlag && keycode == "Super_L"){
//        winFlag = false;
//        return;
//    }else if(m_winFlag && keycode == "Super_L")
//        return;

//    if((keycode == "Super_L") || (keycode == "Super_R"))
//    {
//        //        if(this->isVisible())
//        if(QApplication::activeWindow() == this)
//        {
//            this->hide();
//            m_mainViewWid->widgetMakeZero();
//            //            m_sideBarWid->widgetMakeZero();
//        }
//        else{
//            this->loadMainWindow();
//            this->show();
//            this->raise();
//            this->activateWindow();
//        }
//    }

//    if(keycode == "Escape")
//    {
//        this->hide();
//        m_mainViewWid->widgetMakeZero();
//        //        m_sideBarWid->widgetMakeZero();
//    }
}

void MainWindow::winKeyReleaseSlot(const QString &key)
{
#if 0
    if(key=="winKeyRelease" || key=="win-key-release")
    {
        QGSettings gsetting(QString("org.ukui.session").toLocal8Bit());
        if(gsetting.get(QString("win-key-release")).toBool())
        {
            disconnect(XEventMonitor::instance(), SIGNAL(keyRelease(QString)),
                       this,SLOT(XkbEventsRelease(QString)));
            disconnect(XEventMonitor::instance(), SIGNAL(keyPress(QString)),
                       this,SLOT(XkbEventsPress(QString)));
        }
        else
        {
            connect(XEventMonitor::instance(), SIGNAL(keyRelease(QString)),
                    this,SLOT(XkbEventsRelease(QString)));
            connect(XEventMonitor::instance(), SIGNAL(keyPress(QString)),
                    this,SLOT(XkbEventsPress(QString)));
        }
    }
#endif
}

void MainWindow::loadMainWindow()
{
    qDebug()<<"loadMainWindow";
    QDesktopWidget* m = QApplication::desktop();
    QRect desk_rect = m->screenGeometry(m->screenNumber(QCursor::pos()));
    int desk_x = desk_rect.width();
    int desk_y = desk_rect.height();
    int x = this->width();
    int y = this->height();
    this->move(desk_x / 2 - x / 2 + desk_rect.left(), desk_y / 2 - y / 2 + desk_rect.top());
    m_mainViewWid->loadMinMainView();

    setFrameStyle();
}

void MainWindow::monitorResolutionChange(QRect rect)
{
    Q_UNUSED(rect);
    repaintWidget();
}

void MainWindow::primaryScreenChangedSlot(QScreen *screen)
{
    Q_UNUSED(screen);
    repaintWidget();

}

void MainWindow::repaintWidget()
{
#if 0
    Style::initWidStyle();
    this->setMinimumSize(Style::minw,Style::minh);
    m_line->setFixedSize(1,this->height());
    m_mainViewWid->repaintWidget();

    if(QApplication::activeWindow() == this)
    {
        int position=0;
        int panelSize=0;
        if(QGSettings::isSchemaInstalled(QString("org.ukui.panel.settings").toLocal8Bit()))
        {
            QGSettings* gsetting=new QGSettings(QString("org.ukui.panel.settings").toLocal8Bit());
            if(gsetting->keys().contains(QString("panelposition")))
                position=gsetting->get("panelposition").toInt();
            else
                position=0;
            if(gsetting->keys().contains(QString("panelsize")))
                panelSize=gsetting->get("panelsize").toInt();
            else
                panelSize=46;
        }
        else
        {
            position=0;
            panelSize=46;
        }
        int x=QApplication::primaryScreen()->geometry().x();
        int y=QApplication::primaryScreen()->geometry().y();
        if(position==0)
            this->setGeometry(QRect(x,y+QApplication::primaryScreen()->geometry().height()-panelSize-Style::minh,
                                    Style::minw,Style::minh));
        else if(position==1)
            this->setGeometry(QRect(x,y+panelSize,Style::minw,Style::minh));
        else if(position==2)
            this->setGeometry(QRect(x+panelSize,y,Style::minw,Style::minh));
        else
            this->setGeometry(QRect(x+QApplication::primaryScreen()->geometry().width()-panelSize-Style::minw,y,
                                    Style::minw,Style::minh));

        //            QHBoxLayout *mainLayout=qobject_cast<QHBoxLayout*>(this->centralWidget()->layout());
        //            mainLayout->insertWidget(1,m_line);
        m_mainViewWid->resizeControl();
        setFrameStyle();
    }
#endif
}

void MainWindow::setFrameStyle()
{
#if 1
    char style[100];

    QString m_defaultBackground;
    if(QGSettings::isSchemaInstalled(QString("org.ukui.control-center.personalise").toLocal8Bit()))
    {
        QGSettings* gsetting=new QGSettings(QString("org.ukui.control-center.personalise").toLocal8Bit());
        if(gsetting->keys().contains(QString("transparency")))
        {
            double transparency=gsetting->get("transparency").toDouble();
            m_defaultBackground=QString("rgba(19, 19, 20,"+QString::number(transparency)+")");
        }
        else
            m_defaultBackground=QString("rgba(19, 19, 20, 0.7)");
    }
    else
        m_defaultBackground=QString("rgba(19, 19, 20, 0.7)");

    //不是全屏的情况
    QRectF rect;
    rect.setX(this->rect().x()+1);
    rect.setY(this->rect().y()+1);
    rect.setWidth(this->rect().width()-2);
    rect.setHeight(this->rect().height()-2);
    const qreal radius = 6;
    QPainterPath path;
    //样式
    sprintf(style, "border:0px;background-color:%s;border-top-right-radius:6px;",m_defaultBackground.toLocal8Bit().data());
    path.moveTo(rect.topRight() - QPointF(radius, 0));
    path.lineTo(rect.topLeft());
    path.lineTo(rect.bottomLeft());
    path.lineTo(rect.bottomRight());
    path.lineTo(rect.topRight() + QPointF(0, radius));
    path.quadTo(rect.topRight(), rect.topRight() + QPointF(-radius, -0));
    setProperty("blurRegion", QRegion(path.toFillPolygon().toPolygon()));
    m_frame->setStyleSheet(style);//跟主题变化的style，暂时先不用，先设定透明
    //     m_frame->setStyleSheet("border:0px;background:transparent;");
#endif
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
#if 0
    if(e->type()==KeyPress+4)
    {
        QKeyEvent* ke=static_cast<QKeyEvent*>(e);
        if((ke->key()>=0x30 && ke->key()<=0x39) || (ke->key()>=0x41 && ke->key()<=0x5a))
        {
            m_mainViewWid->setLineEditFocus(e->text());
        }
    }
#endif
}
