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

#include "mainwindow.h"
#include <QDesktopWidget>
#include <QFile>
#include <QScreen>
#include <QTranslator>
#include <QLocale>
#include <X11/Xlib.h>
#include <syslog.h>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
#include <ukui-log4qt.h>
#endif
#include <QObject>
#include <QApplication>
#include "qt-single-application.h"
#include "qt-local-peer.h"
//#include "inotify-manager.h"
#include "libsearch.h"
#include "global-settings.h"
#include "ukui-search-dbus-service.h"

using namespace Zeeker;
//void handler(int){
//    qDebug() << "Recieved SIGTERM!";

//    GlobalSettings::getInstance()->setValue(INDEX_DATABASE_STATE, "2");
//    GlobalSettings::getInstance()->setValue(CONTENT_INDEX_DATABASE_STATE, "2");
//    GlobalSettings::getInstance()->setValue(INDEX_GENERATOR_NORMAL_EXIT, "2");
//    GlobalSettings::getInstance()->setValue(INOTIFY_NORMAL_EXIT, "2");
//    GlobalSettings::getInstance()->forceSync(INDEX_DATABASE_STATE);
//    GlobalSettings::getInstance()->forceSync(CONTENT_INDEX_DATABASE_STATE);
//    GlobalSettings::getInstance()->forceSync(INDEX_GENERATOR_NORMAL_EXIT);
//    GlobalSettings::getInstance()->forceSync(INOTIFY_NORMAL_EXIT);

//    qDebug() << "indexDataBaseStatus: " << GlobalSettings::getInstance()->getValue(INDEX_DATABASE_STATE).toString();
//    qDebug() << "contentIndexDataBaseStatus: " << GlobalSettings::getInstance()->getValue(CONTENT_INDEX_DATABASE_STATE).toString();

//    ::exit(0);

//    InotifyIndex::getInstance("/home")->~InotifyIndex();

//    //wait linux kill this thread forcedly
//    while (true);
//}


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

void centerToScreen(QWidget* widget) {
    if(!widget)
        return;
    QDesktopWidget* m = QApplication::desktop();
    QRect desk_rect = m->screenGeometry(m->screenNumber(QCursor::pos()));
    int desk_x = desk_rect.width();
    int desk_y = desk_rect.height();
    int x = widget->width();
    int y = widget->height();
    widget->move(desk_x / 2 - x / 2 + desk_rect.left(), desk_y / 2 - y / 2 + desk_rect.top());
}
/*
void searchMethod(FileUtils::SearchMethod sm){
    qWarning() << "searchMethod start: " << static_cast<int>(sm);
    if (FileUtils::SearchMethod::INDEXSEARCH == sm || FileUtils::SearchMethod::DIRECTSEARCH == sm) {
        FileUtils::searchMethod = sm;
    } else {
        printf("enum class error!!!\n");
        qWarning("enum class error!!!\n");
    }
    if (FileUtils::SearchMethod::INDEXSEARCH == sm && 0 == FileUtils::indexStatus) {
        qWarning() << "start first index";
        FirstIndex fi("/home/zhangzihao/Desktop");
        fi.start();
        qWarning() << "start inotify index";
//        InotifyIndex ii("/home");
//        ii.start();
        InotifyIndex* ii = InotifyIndex::getInstance("/home");
        if (!ii->isRunning()) {
            ii->start();
        }
        qDebug()<<"Search method has been set to INDEXSEARCH";
    }
    qWarning() << "searchMethod end: " << static_cast<int>(FileUtils::searchMethod);
}
*/
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
    qRegisterMetaType<QPair<QString, QStringList>>("QPair<QString,QStringList>");
    qRegisterMetaType<Document>("Document");

    // If qt version bigger than 5.12, enable high dpi scaling and use high dpi pixmaps?
#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif

    // Make sure only one ukui-search is running.
    QtSingleApplication app("ukui-search", argc, argv);
    app.setQuitOnLastWindowClosed(false);

    if(app.isRunning()) {
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
    /*
        // Create a fifo at ~/.config/org.ukui/ukui-search, the fifo is used to control the order of child processes' running.
        QDir fifoDir = QDir(QDir::homePath()+"/.config/org.ukui/ukui-search");
        if(!fifoDir.exists())
            qDebug()<<"create fifo path"<<fifoDir.mkpath(fifoDir.absolutePath());

        unlink(UKUI_SEARCH_PIPE_PATH);
        int retval = mkfifo(UKUI_SEARCH_PIPE_PATH, 0777);
        if(retval == -1)
        {
            qCritical()<<"creat fifo error!!";
            syslog(LOG_ERR,"creat fifo error!!\n");
            assert(false);
            return -1;
        }
        qDebug()<<"create fifo success\n";
    */
    // Set max_user_watches to a number which is enough big.
    UkuiSearchQDBus usQDBus;
    usQDBus.setInotifyMaxUserWatches();

    // load chinese character and pinyin file to a Map
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

    // Load translations
    QTranslator translator;
    try {
        if(! translator.load("/usr/share/ukui-search/translations/" + QLocale::system().name())) throw - 1;
        app.installTranslator(&translator);
    } catch(...) {
        qDebug() << "Load translations file" << QLocale() << "failed!";
    }

    QTranslator qt_translator;
    try {
        //if(! qt_translator.load(":/res/qt-translations/qt_zh_CN.qm")) throw - 1;
        if(! qt_translator.load("/usr/share/qt5/translations/qt_" + QLocale::system().name())) throw - 1;
        app.installTranslator(&qt_translator);
    } catch(...) {
        qDebug() << "Load translations file" << QLocale() << "failed!";
    }

    //set main window to the center of screen
    MainWindow *w = new MainWindow;
    UkuiSearchDbusServices dbusService(w);
    qApp->setWindowIcon(QIcon::fromTheme("kylin-search"));
//    centerToScreen(w);
//    w->moveToPanel();
    centerToScreen(w);

    //请务必在connect之后初始化mainwindow的Gsettings，为了保证gsettings第一次读取到的配置值能成功应用
    w->initGsettings();

    //使用窗管的无边框策略
//    w->setProperty("useStyleWindowManager", false); //禁用拖动
//    MotifWmHints hints;
//    hints.flags = MWM_HINTS_FUNCTIONS|MWM_HINTS_DECORATIONS;
//    hints.functions = MWM_FUNC_ALL;
//    hints.decorations = MWM_DECOR_BORDER;
//    XAtomHelper::getInstance()->setWindowMotifHint(w->winId(), hints);

    app.setActivationWindow(w);

    // Processing startup parameters
    if(QString::compare(QString("-s"), QString(QLatin1String(argv[1]))) == 0) {
//        w->moveToPanel();
        centerToScreen(w);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
        XAtomHelper::getInstance()->setWindowMotifHint(w->winId(), w->m_hints);
#endif
        w->show();
    }

    QObject::connect(&app, &QtSingleApplication::messageReceived, w, &MainWindow::bootOptionsFilter);

    // Start app search thread
    AppMatch::getAppMatch()->start();

    // TODO
    // Set threads which in global thread pool expiry time in 5ms, some prolems here
//    QThreadPool::globalInstance()->setExpiryTimeout(5);

    // TODO
    // First insdex start, the parameter us useless, should remove the parameter
//    FirstIndex fi("/home/zhangzihao/Desktop");
//    fi.start();

    // TODO
    // Inotify index start, the parameter us useless, should remove the parameter
//    InotifyIndex* ii = InotifyIndex::getInstance("/home");
//    ii->start();

    return app.exec();
}
