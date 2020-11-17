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

#include "src/MainWindow/mainwindow.h"
#include <QtSingleApplication>
#include <QDesktopWidget>
#include <QFile>
#include <QScreen>
#include <QTranslator>
#include <QLocale>
#include <KWindowEffects>
#include <X11/Xlib.h>
#include <syslog.h>
#include <QObject>
#include "qtsingleapplication.h"
#include "qtlocalpeer.h"

void centerToScreen(QWidget* widget) {
    if (!widget)
      return;
    QDesktopWidget* m = QApplication::desktop();
    QRect desk_rect = m->screenGeometry(m->screenNumber(QCursor::pos()));
    int desk_x = desk_rect.width();
    int desk_y = desk_rect.height();
    int x = widget->width();
    int y = widget->height();
    widget->move(desk_x / 2 - x / 2 + desk_rect.left(), desk_y / 2 - y / 2 + desk_rect.top());
}

int main(int argc, char *argv[])
{
    qRegisterMetaType<QVector<QStringList>>("QVector<QStringList>");

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QtSingleApplication app("ukui-menu-search", argc, argv);
    app.setQuitOnLastWindowClosed(true);


    if(app.isRunning())
    {
//        app.sendMessage("raise_window_noop");
//        return EXIT_SUCCESS;

        app.sendMessage(QApplication::arguments().length() > 1 ? QApplication::arguments().at(1) : app.applicationFilePath());
        qDebug() << QObject::tr("ukui-search is already running!");
        return EXIT_SUCCESS;
    }else {
        QCommandLineParser parser;
        QCommandLineOption debugOption({"d", "debug"}, QObject::tr("Display debug information"));
        QCommandLineOption showsearch({"s", "show"}, QObject::tr("show search widget"));

        parser.addOptions({debugOption, showsearch});
        parser.process(app);
    }

    QTranslator translator;
    if (translator.load(QLocale(), "ukui-menu", "_", QM_FILES_INSTALL_PATH))
        app.installTranslator(&translator);
    else
        qDebug() << "Load translations file" << QLocale() << "failed!";

    MainWindow *w=new MainWindow;
    w->setFrameStyle();
    centerToScreen(w);
    w->show();
    w->raise();
    w->activateWindow();
    w->loadMainWindow();
    app.setActivationWindow(w);
    QObject::connect(&app, SIGNAL(messageReceived(const QString&)),w, SLOT(bootOptionsFilter(const QString&)));

//    KWindowEffects::enableBlurBehind(w->winId(),true);

    return app.exec();


//    /* 如果系统中有实例在运行则退出 */
//    QtSingleApplication a(argc, argv);
//    if (a.isRunning()) {
//        a.sendMessage(QApplication::arguments().length() > 1 ? QApplication::arguments().at(1) : a.applicationFilePath());
//        qDebug() << QObject::tr("ukui-sidebar is already running!");
//        return EXIT_SUCCESS;
//    } else {
//        QCommandLineParser parser;
//        QCommandLineOption debugOption({"d", "debug"}, QObject::tr("Display debug information"));
//        QCommandLineOption showSidebar({"s", "show"}, QObject::tr("show sidebar widget"));

//        parser.addOptions({debugOption, showSidebar});
//        parser.process(a);

//        if (parser.isSet(debugOption)) {                                    /* 根据命令行设定日志等级 */
//            setLogLevel(QtDebugMsg);
//            setLogPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + LOG_FILE_NAME);  /* 绑定打印日志文件路径 */
//            qInstallMessageHandler(customLogMessageHandler);
//        } else {
//            setLogLevel(QtWarningMsg);
//        }

//        QApplication::setQuitOnLastWindowClosed(false);

//        mostGrandWidget::mostGrandWidgetInit();                         /* 初始化最里层Widget空白界面 */


//        Widget *w = new Widget;
//        w->setObjectName("SidebarWidget");
//        w->setAttribute(Qt::WA_TranslucentBackground);
//        w->setFixedWidth(392);
//        mostGrandWidget::getInstancemostGrandWidget()->m_pmostGrandWidgetVLaout->addWidget(w);

//        mostGrandWidget::getInstancemostGrandWidget()->LaoutSidebarWidget();
//        mostGrandWidget::getInstancemostGrandWidget()->setMostGrandwidgetCoordinates(-500, 0);
//        mostGrandWidget::getInstancemostGrandWidget()->setProperty("useSystemStyleBlur", true);
//        mostGrandWidget::getInstancemostGrandWidget()->setVisible(true);
//    //    KWindowEffects::enableBlurBehind(mostGrandWidget::getInstancemostGrandWidget()->winId(), true);
//        w->m_bfinish = true;
//        w->showAnimation();
//        QObject::connect(&a, SIGNAL(messageReceived(const QString&)),w, SLOT(bootOptionsFilter(const QString&)));
//        return a.exec();
//    }
}
