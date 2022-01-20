/*
 *
 * Copyright (C) 2020, KylinSoft Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: zhangjiaping <zhangjiaping@kylinos.cn>
 * Modified by: zhangpengfei <zhangpengfei@kylinos.cn>
 * Modified by: zhangzihao <zhangzihao@kylinos.cn>
 *
 */


#include <QDesktopWidget>
#include <QFile>
#include <QDir>
#include <syslog.h>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
#include <ukui-log4qt.h>
#endif
#include <QObject>
#include <QApplication>
#include <QX11Info>
#include "ukui-search-gui.h"

using namespace UkuiSearch;

void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    QByteArray currentTime = QTime::currentTime().toString().toLocal8Bit();

    bool showDebug = true;
//    QString logFilePath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/ukui-search.log";
//    QString logFilePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.config/org.ukui/ukui-search/ukui-search.log";
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

int main(int argc, char *argv[]) {
//v101日志模块
//#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
//    //Init log module
//    initUkuiLog4qt("ukui-search");
//#endif

    // Determine whether the home directory has been created, and if not, keep waiting.
    char *p_home = NULL;

    unsigned int i = 0;
    while(p_home == NULL) {
        ::sleep(1);
        ++i;
        p_home = getenv("HOME");
        if(i % 5 == 0) {
            qWarning() << "I can't find home! I'm done here!!";
            printf("I can't find home! I'm done here!!");
            syslog(LOG_ERR, "I can't find home! I'm done here!!\n");
        }
    }
    p_home = NULL;
    while(!QDir(QDir::homePath()).exists()) {
        qWarning() << "Home is not exits!!";
        printf("Home is not exits!!");
        syslog(LOG_ERR, "Home is not exits!!\n");
        ::sleep(1);
    }

    // Output log to file
    qInstallMessageHandler(messageOutput);
//若使用v101日志模块，可以解放如下判断条件
//#if (QT_VERSION < QT_VERSION_CHECK(5, 12, 0))
//    // Output log to file
//    qInstallMessageHandler(messageOutput);
//#endif

    // Register meta type
    qDebug() << "ukui-search main start";
    // If qt version bigger than 5.12, enable high dpi scaling and use high dpi pixmaps?
#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
    UkuiSearchGui app(argc, argv, QString("ukui-search-gui-%1").arg(QX11Info::appScreen()));
    if (app.isRunning())
        return 0;

    return app.exec();
}
