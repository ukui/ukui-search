#ifndef APPINFOTABLEPRIVATE_H
#define APPINFOTABLEPRIVATE_H
#include <QObject>
#include <QSqlDatabase>
#include <app-info-table.h>

namespace UkuiSearch {
class AppInfoTablePrivate : public QObject
{
    Q_OBJECT
public:
    explicit AppInfoTablePrivate(AppInfoTable *parent = nullptr);
    AppInfoTablePrivate(AppInfoTablePrivate &) = delete;
    AppInfoTablePrivate &operator =(const AppInfoTablePrivate &) = delete;

    bool setAppFavoritesState(QString &desktopfp, size_t num);
    bool setAppTopState(QString &desktopfp, size_t num);
    bool setAppLaunchTimes(QString &desktopfp, size_t num);
    bool updateAppLaunchTimes(QString &desktopfp);
    bool setAppLockState(QString &desktopfp, size_t num);

    bool getAllAppDesktopList(QStringList &list);
    bool getFavoritesAppList(QStringList &list);
    bool getTopAppList(QStringList &list);
    bool getLaunchTimesAppList(QStringList &list);
    bool getAppCategory(QString &desktopfp, QString &category);

    bool getAppInfoResults(QVector<AppInfoTable::AppInfoResult> &appInfoResults);

    bool getAppLockState(QString &desktopfp, size_t &num);
    bool getAppTopState(QString &desktopfp, size_t &num);
    bool getAppLaunchedState(QString &desktopfp, size_t &num);
    bool getAppFavoriteState(QString &desktopfp, size_t &num);

    bool addAppShortcut2Desktop(QString &desktopfp);
    bool addAppShortcut2Panel(QString &desktopfp);

    bool getInstallAppMap(QMultiMap<QString, QStringList> &installAppMap);
    bool searchInstallApp(QString &keyWord, QStringList &installAppInfoRes);
    bool searchInstallApp(QStringList &keyWord, QStringList &installAppInfoRes);

    bool uninstallApp(QString &desktopfp);

    QString lastError(void) const;

private:
    ~AppInfoTablePrivate();
    bool initDateBaseConnection();
    bool openDataBase();
    void closeDataBase();

    AppInfoTable *q = nullptr;
    QSqlDatabase *m_database = nullptr;
    QString m_ConnectionName;

};


}
#endif // APPINFOTABLEPRIVATE_H
