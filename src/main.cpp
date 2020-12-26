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

#include "src/mainwindow.h"
#include <QtSingleApplication>
#include <QDesktopWidget>
#include <QFile>
#include <QScreen>
#include <QTranslator>
#include <QLocale>
#include <X11/Xlib.h>
#include <syslog.h>
#include <QObject>
#include "qt-single-application.h"
#include "qt-local-peer.h"
//#include "inotify-manager.h"
#include "inotify.h"

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
    //load chinese character and pinyin file to a Map
    FileUtils::loadHanziTable("://index/pinyinWithoutTone.txt");
    /*-------------InotyifyRefact Test Start---------------*/
    QTime t1 = QTime::currentTime();
    InotifyManagerRefact* imr = new InotifyManagerRefact("/home");
    imr->start();
    QTime t2 = QTime::currentTime();
    qDebug() << t1;
    qDebug() << t2;
    /*-------------InotyifyRefact Test End-----------------*/

    qRegisterMetaType<QVector<QStringList>>("QVector<QStringList>");

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QtSingleApplication app("ukui-search", argc, argv);
    app.setQuitOnLastWindowClosed(true);


    if(app.isRunning())
    {
        app.sendMessage(QApplication::arguments().length() > 1 ? QApplication::arguments().at(1) : app.applicationFilePath());
        qDebug() << QObject::tr("ukui-search is already running!");
        return EXIT_SUCCESS;
    }/*else {
        QCommandLineParser parser;
        QCommandLineOption debugOption({"d", "debug"}, QObject::tr("Display debug information"));
        QCommandLineOption showsearch({"s", "show"}, QObject::tr("show search widget"));
        parser.addOptions({debugOption, showsearch});
        parser.process(app);
    }*/

    // 加载国际化文件
    QTranslator translator;
    try {
        if (! translator.load("/usr/share/ukui-search/res/translations/" + QLocale::system().name())) throw -1;
        app.installTranslator(&translator);
    } catch (...) {
        qDebug() << "Load translations file" << QLocale() << "failed!";
    }

    MainWindow *w = new MainWindow;
    QStringList arguments = QCoreApplication::arguments();
    centerToScreen(w);
    w->show();
    w->raise();
    w->activateWindow();
//    w->loadMainWindow();
    app.setActivationWindow(w);
    if(arguments.size()>1)
    w->searchContent(arguments.at(1));
    QObject::connect(&app, SIGNAL(messageReceived(const QString&)),w, SLOT(bootOptionsFilter(const QString&)));


    return app.exec();
}
