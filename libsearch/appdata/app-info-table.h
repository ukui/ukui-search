#ifndef APPINFOTABLE_H
#define APPINFOTABLE_H

#include <QObject>
#include "app-db-common.h"

namespace UkuiSearch {

class AppInfoTablePrivate;
class AppInfoTable : public QObject
{
    Q_OBJECT
public:
    explicit AppInfoTable(QObject *parent = nullptr);
    AppInfoTable(AppInfoTable &) = delete;
    AppInfoTable &operator =(const AppInfoTable &) = delete;

    /**
     * @brief AppInfoTable::setAppFavoritesState
     * set the favorites state of the app
     * @param desktopfp: the desktop file path of app
     * @param num: the favorites app's order(from 1)
     * @return bool: true if success, else false
     */
    bool setAppFavoritesState(QString &desktopfp, size_t num);

    /**
     * @brief AppInfoTable::setAppTopState
     * set the top state of the app
     * @param desktopfp: the desktop file path of app
     * @param num: the top app's order(from 1)
     * @return bool: true if success, else false
     */
    bool setAppTopState(QString &desktopfp, size_t num);

    /**
     * @brief AppInfoTable::getAllAppDesktopList
     * get the desktop file paths of all apps
     * @param list: a stringlist include all paths
     * @return bool: true if success, else false
     */
    bool getAllAppDesktopList(QStringList &list);

    /**
     * @brief AppInfoTable::getFavoritesAppList
     * get all favorites apps
     * @param list: a stringlist include all apps' paths
     * @return bool: true if success, else false
     */
    bool getFavoritesAppList(QStringList &list);

    /**
     * @brief AppInfoTable::getTopAppList
     * get all top apps
     * @param list: a stringlist include all apps' paths
     * @return bool: true if success, else false
     */
    bool getTopAppList(QStringList &list);

    bool getLaunchTimesAppList(QStringList &list);
    bool getAppCategory(QString &desktopfp, QString &category);

    /**
     * @brief AppInfoTable::changeFavoriteAppPos
     * change the position of the app which is one of the Favorites Apps
     * @param desktopfp: desktop file path of app
     * @param pos: the position which the app will be changed into
     * @return bool: true if success, else false
     */
    bool changeFavoriteAppPos(const QString &desktopfp, size_t pos);

    /**
     * @brief AppInfoTable::changeTopAppPos
     * hange the position of the app which is one of the Top Apps
     * @param desktopfp: desktop file path of app
     * @param pos: the position which the app will be changed into
     * @return bool: true if success, else false
     */
    bool changeTopAppPos(const QString &desktopfp, size_t pos);

    /**
     * @brief AppInfoTable::getAppInfoResults
     * Get all App infos by passing AppInforesult Vector.
     * @param AppInfoResults: a struct which includes all infos of each application
     * @return bool: true if success, else false
     */
    bool getAppInfoResults(QVector<AppInfoResult> &appInfoResults);

    bool getAppLockState(QString &desktopfp, size_t &num);
    bool getAppTopState(QString &desktopfp, size_t &num);
    bool getAppLaunchedState(QString &desktopfp, size_t &num);
    bool getAppFavoriteState(QString &desktopfp, size_t &num);

    bool addAppShortcut2Desktop(QString &desktopfp);
    bool addAppShortcut2Panel(QString &desktopfp);

    bool searchInstallApp(QString &keyWord, QStringList &installAppInfoRes);
    bool searchInstallApp(QStringList &keyWord, QStringList &installAppInfoRes);

    bool uninstallApp(QString &desktopfp);

    /**
     * @brief AppInfoTable::lastError
     * the last error of the database
     * @return QString: the text of the last error
     */
    QString lastError(void) const;

private:
    bool setAppLaunchTimes(QString &desktopfp, size_t num);
    bool setAppLockState(QString &desktopfp, size_t num);
    bool updateAppLaunchTimes(QString &desktopfp);

    AppInfoTablePrivate *d;

Q_SIGNALS:
    void DBOpenFailed();
    void appDBItems2BUpdate(QVector<AppInfoResult>);
    void appDBItems2BAdd(QVector<AppInfoResult>);
    void appDBItems2BDelete(QStringList);
};
}

#endif // APPINFOTABLE_H
