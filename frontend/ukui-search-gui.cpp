#include "ukui-search-gui.h"
#include <QScreen>
#include <QTranslator>
#include <QLocale>
#include <QApplication>
#include <QCommandLineParser>
#include "plugin-manager.h"
#include "search-plugin-manager.h"
#include "document.h"

using namespace UkuiSearch;
UkuiSearchGui::UkuiSearchGui(int &argc, char *argv[], const QString &applicationName): QtSingleApplication (applicationName, argc, argv)
{
    qDebug()<<"ukui search gui constructor start" << applicationName;
    setApplicationVersion(QString("v%1").arg(VERSION));
    if (!this->isRunning()) {
        connect(this, &QtSingleApplication::messageReceived, [=](QString msg) {
            this->parseCmd(msg, true);
        });

        setQuitOnLastWindowClosed(false);

//        qRegisterMetaType<QPair<QString, QStringList>>("QPair<QString,QStringList>");
//        qRegisterMetaType<UkuiSearch::Document>("Document");


        //load translations.
        QTranslator translator;
        try {
            if(! translator.load("/usr/share/ukui-search/translations/" + QLocale::system().name())) throw - 1;
            this->installTranslator(&translator);
        } catch(...) {
            qDebug() << "Load translations file" << QLocale() << "failed!";
        }

        QTranslator qt_translator;
        try {
            if(! qt_translator.load(":/res/qt-translations/qt_zh_CN.qm")) throw - 1;
            this->installTranslator(&qt_translator);
        } catch(...) {
            qDebug() << "Load translations file" << QLocale() << "failed!";
        }

        QTranslator lib_translator;
        try {
            if(! lib_translator.load("/usr/share/ukui-search/translations/libukui-search_" + QLocale::system().name())) throw - 1;
            this->installTranslator(&lib_translator);
        } catch(...) {
            qDebug() << "Load translations file" << QLocale() << "failed!";
        }

        qDebug() << "Loading plugins and resources...";
        SearchPluginManager::getInstance();
        PluginManager::getInstance();

        m_mainWindow = new UkuiSearch::MainWindow();
        m_dbusService = new UkuiSearch::UkuiSearchDbusServices(m_mainWindow);
        qApp->setWindowIcon(QIcon::fromTheme("kylin-search"));
        this->setActivationWindow(m_mainWindow);
    }

    //parse cmd
    qDebug()<<"parse cmd";
    auto message = this->arguments().join(' ').toUtf8();
    parseCmd(message, !isRunning());

    qDebug()<<"ukui search gui constructor end";
}

UkuiSearchGui::~UkuiSearchGui()
{
    if(m_mainWindow) {
        delete m_mainWindow;
        m_mainWindow = nullptr;
    }
    if(m_dbusService) {
        delete m_dbusService;
        m_dbusService = nullptr;
    }
}

void UkuiSearchGui::parseCmd(QString msg, bool isPrimary)
{
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption quitOption(QStringList()<<"q"<<"quit", tr("Quit ukui-search application"));
    parser.addOption(quitOption);

    QCommandLineOption showOption(QStringList()<<"s"<<"show", tr("Show main window"));
    parser.addOption(showOption);

    if (isPrimary) {
        const QStringList args = QString(msg).split(' ');
        parser.process(args);
        if(parser.isSet(showOption)) {
            m_mainWindow->bootOptionsFilter("-s");
        }
        if (parser.isSet(quitOption)) {
            this->quit();
        }
    }
    else {
        if (arguments().count() < 2) {
            parser.showHelp();
        }
        parser.process(arguments());
        sendMessage(msg);
    }
}
