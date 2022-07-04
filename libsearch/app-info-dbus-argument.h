#ifndef APPINFODBUSARGUMENT_H
#define APPINFODBUSARGUMENT_H

#include <QDBusArgument>
#include "app-db-common.h"

namespace UkuiSearch {

    QDBusArgument &operator << (QDBusArgument &argument, const AppInfoResult &infoResult)
    {
        argument.beginStructure();
        argument << infoResult.desktopPath << infoResult.iconName << infoResult.appLocalName << infoResult.firstLetter
                 << infoResult.category << infoResult.top << infoResult.favorite << infoResult.launchTimes << infoResult.lock;
        argument.endStructure();
        return argument;
    }

    const QDBusArgument &operator >> (const QDBusArgument &argument, AppInfoResult &infoResult)
    {
        argument.beginStructure();
        argument >> infoResult.desktopPath >> infoResult.iconName >> infoResult.appLocalName >> infoResult.firstLetter
                >> infoResult.category >> infoResult.top >> infoResult.favorite >> infoResult.launchTimes >> infoResult.lock;

        argument.endStructure();
        return argument;
    }

}


#endif // APPINFODBUSARGUMENT_H
