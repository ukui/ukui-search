#include "ukui-search-dbus-service.h"

using namespace Zeeker;
void UkuiSearchDbusServices::showWindow(){
    m_mainWindow->bootOptionsFilter("-s");
}

UkuiSearchDbusServices::UkuiSearchDbusServices(MainWindow *m)
{
    m_mainWindow = m;
    //注册服务
    QDBusConnection sessionBus = QDBusConnection::sessionBus();
    if(!sessionBus.registerService("org.ukui.search.service")){
        qWarning() << "ukui-search dbus register service failed reason:" << sessionBus.lastError();
    }

    if(!sessionBus.registerObject("/", this, QDBusConnection::ExportAllSlots)){
        qWarning() << "ukui-search dbus register object failed reason:" << sessionBus.lastError();
    }
}
UkuiSearchDbusServices::~UkuiSearchDbusServices(){
}
