#include <QDebug>
#include "ukui-search-service.h"
#define UKUI_SEARCH_SCHEMAS "org.ukui.search.settings"
#define SEARCH_METHOD_KEY "indexSearch"
using namespace UkuiSearch;
UkuiSearchService::UkuiSearchService(int &argc, char *argv[], const QString &applicationName): QtSingleApplication (applicationName, argc, argv)
{
    qDebug()<<"ukui search service constructor start";
    setApplicationVersion(QString("v%1").arg(VERSION));
    setQuitOnLastWindowClosed(false);

    if (!this->isRunning()) {
        connect(this, &QtSingleApplication::messageReceived, [=](QString msg) {
            this->parseCmd(msg, true);
        });
        initGsettings();
    }

    //parse cmd
    qDebug()<<"parse cmd";
    auto message = this->arguments().join(' ').toUtf8();
    parseCmd(message, !isRunning());

    qDebug()<<"ukui search service constructor end";
}

void UkuiSearchService::parseCmd(QString msg, bool isPrimary)
{
    QCommandLineParser parser;

    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption quitOption(QStringList()<<"q"<<"quit", tr("Stop service"));
    parser.addOption(quitOption);

    QCommandLineOption startOption(QStringList()<<"i"<<"index", tr("start or stop file index"), "option");
    parser.addOption(startOption);

//    QCommandLineOption statusOption(QStringList()<<"s"<<"status", tr("show status of file index service"));
//    parser.addOption(statusOption);

    if (isPrimary) {
        const QStringList args = QString(msg).split(' ');
        parser.process(args);
        if(parser.isSet(startOption)) {
            qDebug() << "options!!!!" << parser.value(startOption);
            if(parser.value(startOption) == "start") {
                indexServiceSwitch(true);
            } else if (parser.value(startOption) == "stop") {
                indexServiceSwitch(false);
            }
        }
//        if(parser.isSet(statusOption)) {
//        }
        if (parser.isSet(quitOption)) {
            qApp->quit();
            return;
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

void UkuiSearchService::initGsettings()
{
    const QByteArray id(UKUI_SEARCH_SCHEMAS);
    if(QGSettings::isSchemaInstalled(id)) {
        m_SearchGsettings = new QGSettings(id);
        connect(m_SearchGsettings, &QGSettings::changed, this, [ = ](const QString &key) {
            if(key == SEARCH_METHOD_KEY) {
                setSearchMethodByGsettings();
            }
        });
        if(m_SearchGsettings->keys().contains(SEARCH_METHOD_KEY)) {
            setSearchMethodByGsettings();
        }
    } else {
        qWarning() << UKUI_SEARCH_SCHEMAS << " is not found!";
    }
}

void UkuiSearchService::setSearchMethodByGsettings()
{
    bool isIndexSearch = m_SearchGsettings->get(SEARCH_METHOD_KEY).toBool();
    if(isIndexSearch) {
        FileUtils::searchMethod = FileUtils::SearchMethod::INDEXSEARCH;
    } else {
        FileUtils::searchMethod = FileUtils::SearchMethod::DIRECTSEARCH;
    }
    SearchMethodManager::getInstance()->searchMethod(FileUtils::searchMethod);
}

void UkuiSearchService::indexServiceSwitch(bool startIndex)
{
    if(startIndex) {
        FileUtils::searchMethod = FileUtils::SearchMethod::INDEXSEARCH;
    } else {
        FileUtils::searchMethod = FileUtils::SearchMethod::DIRECTSEARCH;
    }
    SearchMethodManager::getInstance()->searchMethod(FileUtils::searchMethod);

    const QByteArray id(UKUI_SEARCH_SCHEMAS);
    if(QGSettings::isSchemaInstalled(id)) {
        m_SearchGsettings = new QGSettings(id);
        if(m_SearchGsettings->keys().contains(SEARCH_METHOD_KEY)) {
            m_SearchGsettings->set(SEARCH_METHOD_KEY, startIndex);
        } else {
            qWarning() << SEARCH_METHOD_KEY << " is not found!";
        }
    } else {
        qWarning() << UKUI_SEARCH_SCHEMAS << " is not found!";
    }
}
