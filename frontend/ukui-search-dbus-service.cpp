#include "ukui-search-dbus-service.h"

using namespace Zeeker;
void UkuiSearchDbusServices::showWindow(){
    qDebug() << "showWindow called";
    m_mainWindow->bootOptionsFilter("-s");
}

void UkuiSearchDbusServices::hideWindow(){
    qDebug() << "hideWindow called";
    m_mainWindow->hideMainWindow();
}

void UkuiSearchDbusServices::searchKeyword(QString keyword)
{
    showWindow();
    m_mainWindow->setText(keyword);
}

UkuiSearchDbusServices::UkuiSearchDbusServices(MainWindow *m)
{
    m_mainWindow = m;
    //注册服务
    QDBusConnection sessionBus = QDBusConnection::sessionBus();
    QDBusConnection::sessionBus().unregisterService("com.ukui.search.service");
    if (!sessionBus.registerService("com.ukui.search.service")){
        qWarning() << "ukui-search dbus register service failed reason:" << sessionBus.lastError();
    }

    if (!sessionBus.registerObject("/", this, QDBusConnection::ExportAllSlots)){
        qWarning() << "ukui-search dbus register object failed reason:" << sessionBus.lastError();
    }
}

UkuiSearchDbusServices::~UkuiSearchDbusServices(){
}
