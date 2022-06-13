#ifndef APPDBCOMMONDEFINES_H
#define APPDBCOMMONDEFINES_H
#include <QDir>

namespace UkuiSearch {

#define APP_DATABASE_PATH QDir::homePath()+"/.config/org.ukui/ukui-search/appdata/"
#define APP_DATABASE_NAME "app-info.db"

}
#endif // APPDBCOMMONDEFINES_H
