#ifndef APPDBCOMMON_H
#define APPDBCOMMON_H

#include <QDir>
#include <QMetaType>
#include <QDBusArgument>

namespace UkuiSearch {

#define APP_DATABASE_PATH QDir::homePath()+"/.config/org.ukui/ukui-search/appdata/"
#define APP_DATABASE_NAME "app-info.db"

struct AppInfoResult
{
    QString desktopPath;
    QString iconName;
    QString appLocalName;
    QString firstLetter;
    QString category;
    int top;
    int favorite;
    int launchTimes;
    int lock;

    AppInfoResult() : top(0), favorite(0), launchTimes(0), lock(0) {}
    friend QDBusArgument &operator << (QDBusArgument &argument, const AppInfoResult &infoResult)
    {
        argument.beginStructure();
        argument << infoResult.desktopPath << infoResult.iconName << infoResult.appLocalName << infoResult.firstLetter
                 << infoResult.category << infoResult.top << infoResult.favorite << infoResult.launchTimes << infoResult.lock;
        argument.endStructure();
        return argument;
    }

    friend const QDBusArgument &operator >> (const QDBusArgument &argument, AppInfoResult &infoResult)
    {
        argument.beginStructure();
        argument >> infoResult.desktopPath >> infoResult.iconName >> infoResult.appLocalName >> infoResult.firstLetter
                 >> infoResult.category >> infoResult.top >> infoResult.favorite >> infoResult.launchTimes >> infoResult.lock;

        argument.endStructure();
        return argument;
    }

};

}

Q_DECLARE_METATYPE(UkuiSearch::AppInfoResult)

#endif // APPDBCOMMON_H
