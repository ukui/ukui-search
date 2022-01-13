/*
 * Copyright (C) 2022, KylinSoft Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: zhangpengfei <zhangpengfei@kylinos.cn>
 *
 */
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
    QDBusConnection::sessionBus().unregisterService("com.ukui.search.service");
    if(!sessionBus.registerService("com.ukui.search.service")){
        qWarning() << "ukui-search dbus register service failed reason:" << sessionBus.lastError();
    }

    if(!sessionBus.registerObject("/", this, QDBusConnection::ExportAllSlots)){
        qWarning() << "ukui-search dbus register object failed reason:" << sessionBus.lastError();
    }
}
UkuiSearchDbusServices::~UkuiSearchDbusServices(){
}
