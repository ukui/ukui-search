#ifndef APPDBCOMMON_H
#define APPDBCOMMON_H

#include <QDir>

namespace UkuiSearch {

#define APP_DATABASE_PATH QDir::homePath()+"/.config/org.ukui/ukui-search/appdata/"
#define APP_DATABASE_NAME "app-info.db"

struct AppInfoResult
{
public:
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
};
}

Q_DECLARE_METATYPE(UkuiSearch::AppInfoResult)

#endif // APPDBCOMMON_H
