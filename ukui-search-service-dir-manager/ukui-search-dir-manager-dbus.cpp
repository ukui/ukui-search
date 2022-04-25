#include "ukui-search-dir-manager-dbus.h"
#include "dir-watcher.h"

#include <QDBusConnection>
#include <QDBusError>
#include <QDebug>

UkuiSearchDirManagerDbus::UkuiSearchDirManagerDbus(int &argc, char *argv[], const QString &applicationName) : QtSingleApplication (applicationName, argc, argv)
{
    setQuitOnLastWindowClosed(false);
    setApplicationVersion(QString("v%1").arg(VERSION));

    if (!this->isRunning()) {
        qDebug() << "I'm in dir manager dbus rigister";
        QDBusConnection sessionBus = QDBusConnection::sessionBus();
        if(!sessionBus.registerService("com.ukui.search.fileindex.service")) {
            qCritical() << "QDbus register service failed reason:" << sessionBus.lastError();
        }

        if(!sessionBus.registerObject("/org/ukui/search/privateDirWatcher", DirWatcher::getDirWatcher(), QDBusConnection::ExportNonScriptableSlots | QDBusConnection::ExportAllSignals)) {
            qCritical() << "ukui-search-fileindex dbus register object failed reason:" << sessionBus.lastError();
        }

        connect(this, &QtSingleApplication::messageReceived, [=](QString msg) {
            this->parseCmd(msg, true);
        });
    } else {
        qWarning() << "Ukui search dir manager is running!";
    }
    //parse cmd
    qDebug()<<"parse cmd";
    auto message = this->arguments().join(' ').toUtf8();
    parseCmd(message, !isRunning());
}

void UkuiSearchDirManagerDbus::parseCmd(QString msg, bool isPrimary)
{
    QCommandLineParser parser;

    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption quitOption(QStringList()<<"q"<<"quit", tr("Quit DBus service"));
    parser.addOption(quitOption);

     if (isPrimary) {
         if (parser.isSet(quitOption)) {
             qApp->quit();
             return;
         }
     } else {
         if (arguments().count() < 2) {
             parser.showHelp();
         }
         parser.process(arguments());
         sendMessage(msg);
     }

}
