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
#include "libsearch.h"
#include "global-settings.h"

void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    QByteArray currentTime = QTime::currentTime().toString().toLocal8Bit();

    bool showDebug = true;
//    QString logFilePath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/ukui-search.log";
    QString logFilePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.config/org.ukui/ukui-search.log";
    if (!QFile::exists(logFilePath)) {
        showDebug = false;
    }
    FILE *log_file = nullptr;

    if (showDebug) {
        log_file = fopen(logFilePath.toLocal8Bit().constData(), "a+");
    }

    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    switch (type) {
    case QtDebugMsg:
        if (!log_file) {
            break;
        }
        fprintf(log_file, "Debug: %s: %s (%s:%u, %s)\n", currentTime.constData(), localMsg.constData(), file, context.line, function);
        break;
    case QtInfoMsg:
        fprintf(log_file? log_file: stdout, "Info: %s: %s (%s:%u, %s)\n", currentTime.constData(), localMsg.constData(), file, context.line, function);
        break;
    case QtWarningMsg:
        fprintf(log_file? log_file: stderr, "Warning: %s: %s (%s:%u, %s)\n", currentTime.constData(), localMsg.constData(), file, context.line, function);
        break;
    case QtCriticalMsg:
        fprintf(log_file? log_file: stderr, "Critical: %s: %s (%s:%u, %s)\n", currentTime.constData(), localMsg.constData(), file, context.line, function);
        break;
    case QtFatalMsg:
        fprintf(log_file? log_file: stderr, "Fatal: %s: %s (%s:%u, %s)\n", currentTime.constData(), localMsg.constData(), file, context.line, function);
        break;
    }

    if (log_file)
        fclose(log_file);
}

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

    qInstallMessageHandler(messageOutput);

    qDebug() << "main start";
    FirstIndex* fi = new FirstIndex("/home");
    fi->start();
    /*-------------ukuisearchdbus Test start-----------------*/
//    UkuiSearchQDBus usQDBus;
//    usQDBus.setInotifyMaxUserWatches();

    /*-------------ukuisearchdbus Test End-----------------*/

    //load chinese character and pinyin file to a Map
    FileUtils::loadHanziTable("://index/pinyinWithoutTone.txt");
    /*-------------InotyifyRefact Test Start---------------*/
//    QTime t1 = QTime::currentTime();
//    InotifyManagerRefact* imr = new InotifyManagerRefact("/home");
//    imr->start();
//    QTime t2 = QTime::currentTime();
//    qDebug() << t1;
//    qDebug() << t2;
    /*-------------InotyifyRefact Test End-----------------*/

    /*-------------content index Test Start---------------*/
//    QTime t3 = QTime::currentTime();
//    FileTypeFilter* ftf = new FileTypeFilter("/home");
//    ftf->Test();
//    QTime t4 = QTime::currentTime();
//    delete ftf;
//    ftf = nullptr;
//    qDebug() << t3;
//    qDebug() << t4;
    /*-------------content index Test End-----------------*/
    /*-------------文本搜索 Test start-----------------*/
//    FileSearcher *search = new FileSearcher();
//    search->onKeywordSearchContent("重要器官移植⑤白血病");
//    search->onKeywordSearchContent("g,e,x");
    /*-------------文本搜索 Test End-----------------*/

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QtSingleApplication app("ukui-search", argc, argv);
    app.setQuitOnLastWindowClosed(false);

    qDebug() << "main start x2";

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
        if (! translator.load("/usr/share/ukui-search/translations/" + QLocale::system().name())) throw -1;
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
//    if(arguments.size()>1)
//    w->searchContent(arguments.at(1));
    QObject::connect(&app, SIGNAL(messageReceived(const QString&)),w, SLOT(bootOptionsFilter(const QString&)));


    return app.exec();
}
