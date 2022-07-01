#include "ukui-search-app-data-service.h"
#include "app-db-manager.h"
#include "signal-transformer.h"

#include <QDBusConnection>
#include <QDBusMetaType>
#include <QDBusError>
#include <QDebug>

using namespace UkuiSearch;

UkuiSearchAppDataService::UkuiSearchAppDataService(int &argc, char *argv[], const QString &applicationName):
    QtSingleApplication (applicationName, argc, argv)
{
    qDebug() << "ukui search app data service constructor start";
    setApplicationVersion(QString("v%1").arg(VERSION));
    setQuitOnLastWindowClosed(false);

    if (!this->isRunning()) {
        qDebug() << "First running, I'm in app-db manager dbus rigister.";

        qRegisterMetaType<AppInfoResult>("AppInfoResult");
        qRegisterMetaType<QVector<AppInfoResult>>("QVector<AppInfoResult>");

        qDBusRegisterMetaType<AppInfoResult>();
        qDBusRegisterMetaType<QVector<AppInfoResult>>();

        AppDBManager::getInstance();

        QDBusConnection sessionBus = QDBusConnection::sessionBus();
        if (!sessionBus.registerService("com.ukui.search.appdb.service")) {
            qCritical() << "QDbus register service failed reason:" << sessionBus.lastError();
        }
        if(!sessionBus.registerObject("/org/ukui/search/appDataBase", SignalTransformer::getTransformer(), QDBusConnection::ExportAllSignals)) {
            qCritical() << "ukui-search-fileindex dbus register object failed reason:" << sessionBus.lastError();
        }

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
