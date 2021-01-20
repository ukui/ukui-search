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

void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{

    QString txt;
          switch (type) {
          //调试信息提示
          case QtDebugMsg:
                  txt = QString("Debug: %1").arg(msg);
                  break;

          //一般的warning提示
          case QtWarningMsg:
                  txt = QString("Warning: %1").arg(msg);
          break;
          //严重错误提示
          case QtCriticalMsg:
                  txt = QString("Critical: %1").arg(msg);
          break;
          //致命错误提示
          case QtFatalMsg:
                  txt = QString("Fatal: %1").arg(msg);
                  abort();
          }
    QFile outFile(qgetenv("HOME") +"/.config/ukui/ukui-search.log");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);  //
    ts << txt << endl;
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
    if (translator.load(QLocale(), "ukui-search", "_", QM_FILES_INSTALL_PATH))
        app.installTranslator(&translator);
    else
        qDebug() << "Load translations file" << QLocale() << "failed!";

    MainWindow *w=new MainWindow;
    QStringList arguments = QCoreApplication::arguments();
    centerToScreen(w);
    w->show();
    w->raise();
    w->activateWindow();
    w->loadMainWindow();
    app.setActivationWindow(w);
    if(arguments.size()>1)
    w->searchContent(arguments.at(1));
    QObject::connect(&app, SIGNAL(messageReceived(const QString&)),w, SLOT(bootOptionsFilter(const QString&)));

    KWindowEffects::enableBlurBehind(w->winId(),true);

    //注册MessageHandler
    QFile outFile(qgetenv("HOME") +"/.config/ukui/ukui-search.log");
    outFile.remove();
    qInstallMessageHandler(outputMessage);

    return app.exec();
}
