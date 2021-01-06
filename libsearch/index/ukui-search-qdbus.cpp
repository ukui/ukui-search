#include "ukui-search-qdbus.h"
#include <QDebug>

UkuiSearchQDBus::UkuiSearchQDBus()
{
    this->tmpSystemQDBusInterface = new QDBusInterface("com.ukui.search.qt.systemdbus",
                                                       "/",
                                                       "com.ukui.search.interface",
                                                       QDBusConnection::systemBus());
    if (!tmpSystemQDBusInterface->isValid()){
        qCritical() << "Create Client Interface Failed When execute chage: " << QDBusConnection::systemBus().lastError();
        return;
    }
}
UkuiSearchQDBus::~UkuiSearchQDBus(){
    delete this->tmpSystemQDBusInterface;
    this->tmpSystemQDBusInterface = nullptr;
}

//一键三连
void UkuiSearchQDBus::setInotifyMaxUserWatches()
{
    // /proc/sys/fs/inotify/max_user_watches
    this->tmpSystemQDBusInterface->call("setInotifyMaxUseWatchesStep1");
    // sysctl
    this->tmpSystemQDBusInterface->call("setInotifyMaxUseWatchesStep2");
    // /etc/sysctl.conf
    this->tmpSystemQDBusInterface->call("setInotifyMaxUseWatchesStep3");
}
