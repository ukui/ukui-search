#include <QDebug>
#include "ukui-search-app-data-service.h"
#include "app-db-manager.h"

using namespace UkuiSearch;

UkuiSearchAppDataService::UkuiSearchAppDataService(int &argc, char *argv[], const QString &applicationName):
    QtSingleApplication (applicationName, argc, argv)
{
    qDebug() << "ukui search app data service constructor start";
    setApplicationVersion(QString("v%1").arg(VERSION));
    setQuitOnLastWindowClosed(false);

    if (!this->isRunning()) {
        qDebug() << "First running";
        AppDBManager::getInstance();

        connect(this, &QtSingleApplication::messageReceived, [ & ](QString msg) {
            this->parseCmd(msg, true);
        });
    }

    //parse cmd
    qDebug() << "parse cmd";
    auto message = this->arguments().join(' ').toUtf8();
    parseCmd(message, !isRunning());
    qDebug() << "ukui search app data service constructor end";
}

void UkuiSearchAppDataService::parseCmd(QString msg, bool isPrimary)
{
    QCommandLineParser parser;

    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption quitOption(QStringList()<<"q"<<"quit", tr("Stop service"));
    parser.addOption(quitOption);

    if (isPrimary) {
        const QStringList args = QString(msg).split(' ');
        parser.process(args);

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
