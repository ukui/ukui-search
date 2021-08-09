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
        qCritical() << "QDbus register service failed reason:" << sessionBus.lastError();
        exit(1);
    }

    if(!sessionBus.registerObject("/", this, QDBusConnection::ExportAllSlots)){
        qCritical() << "QDbus register object failed reason:" << sessionBus.lastError();
        exit(2);
    }
}

UkuiSearchDbusServices::~UkuiSearchDbusServices(){
}
