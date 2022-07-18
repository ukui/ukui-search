//
// Created by hxf on 22-7-1.
//

#include "search-application.h"
#include "main-window.h"
#include "plugin-manager.h"

#include <QCommandLineParser>
#include <QTranslator>
#include <QDebug>

SearchApplication::SearchApplication(int &argc, char **argv, int i) : QtSingleApplication(argc, argv, i)
{
    if (!isRunning()) {
        SearchApplication::loadTranslation();
        UkuiSearch::PluginManager::getInstance();
        initMainWindow();

        connect(this, &QtSingleApplication::messageReceived, [=](const QString &msg) {
            this->parseCommand(msg, true);
        });
    }

    parseCommand(this->arguments().join(' ').toUtf8(), !isRunning());
}

void SearchApplication::initMainWindow() {
    if (!m_mainWindow) {
        m_mainWindow = new UkuiSearch::MainWindow();
    }
}

void SearchApplication::parseCommand(const QString &msg, bool isPrimary) {
    QCommandLineParser parser;

    QCommandLineOption showOption = QCommandLineOption(QStringList()<<"s"<<"show", tr("Show Ukui-Search main window"));
    QCommandLineOption hideOption = QCommandLineOption(QStringList()<<"h"<<"hide", tr("Hide Ukui-Search main window"));
    QCommandLineOption quitOption = QCommandLineOption(QStringList()<<"q"<<"quit", tr("Quit Ukui-Search"));

    parser.addOption(showOption);
    parser.addOption(hideOption);
    parser.addOption(quitOption);
    parser.addHelpOption();
    parser.addVersionOption();

    if (isPrimary) {
        const QStringList args = msg.split(' ');
        parser.process(args);

        if (parser.isSet(showOption)) {
            qDebug() << "show main window";
            m_mainWindow->showWindow();
        }
        if (parser.isSet(hideOption)) {
            qDebug() << "hide main window";
            m_mainWindow->hideWindow();
        }
        if (parser.isSet(quitOption)) {
            qDebug() << "quit main window";
            qApp->quit();
        }
    } else {
        if (arguments().count() < 2) {
            parser.showHelp();
        }
        parser.process(arguments());

        sendMessage(msg);
    }
}

SearchApplication::~SearchApplication() {
    if (m_mainWindow) {
        delete m_mainWindow;
        m_mainWindow = nullptr;
    }
}

void SearchApplication::loadTranslation() {
    QTranslator translator;
    try {
        if(! translator.load("/usr/share/ukui-search/translations/" + QLocale::system().name())) {
            throw std::exception();
        }
        SearchApplication::installTranslator(&translator);
    } catch(...) {
        qDebug() << "Load translations file (translator)" << QLocale() << "failed!";
    }

    QTranslator qt_translator;
    try {
        if(! qt_translator.load(":/qt-translations/qt_zh_CN.qm")) {
            throw std::exception();
        }
        SearchApplication::installTranslator(&qt_translator);
    } catch(...) {
        qDebug() << "Load translations file (qt_translator)" << QLocale() << "failed!";
    }

    QTranslator lib_translator;
    try {
        if(! lib_translator.load("/usr/share/ukui-search/translations/libukui-search_" + QLocale::system().name())) {
            throw std::exception();
        }
        SearchApplication::installTranslator(&lib_translator);
    } catch(...) {
        qDebug() << "Load translations file (lib_translator)" << QLocale() << "failed!";
    }
}
