#ifndef APPINFOTABLE_H
#define APPINFOTABLE_H

#include <QObject>
namespace UkuiSearch {
class AppInfoTablePrivate;
class AppInfoTable : public QObject
{
    Q_OBJECT

public:
    typedef struct appInfoResult
    {
        appInfoResult() {
            top = 0;
            favorate = 0;
            launchTimes = 0;
            lock = 0;
        }

        QString desktopPath;
        QString iconName;
        QString appLocalName;
        QString firstLetter;
        QString category;
        size_t top;
        size_t favorate;
        size_t launchTimes;
        size_t lock;

    } AppInfoResult;

public:
    explicit AppInfoTable(QObject *parent = nullptr);
    AppInfoTable(AppInfoTable &) = delete;
    AppInfoTable &operator =(const AppInfoTable &) = delete;

    bool setAppFavoritesState(QString &desktopfp, size_t num);
    bool setAppTopState(QString &desktopfp, size_t num);

    bool getAllAppDesktopList(QStringList &list);
    bool getFavoritesAppList(QStringList &list);
    bool getTopAppList(QStringList &list);
    bool getLaunchTimesAppList(QStringList &list);
    bool getAppCategory(QString &desktopfp, QString &category);

    /**
     * @brief getAppInfoResults
     * @param appInfoResults
     * @return
     */
    bool getAppInfoResults(QVector<AppInfoTable::AppInfoResult> &appInfoResults);

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
     * @brief lastError
     * @return
     */
    QString lastError(void) const;

private:
    bool setAppLaunchTimes(QString &desktopfp, size_t num);
    bool setAppLockState(QString &desktopfp, size_t num);
    bool updateAppLaunchTimes(QString &desktopfp);

    AppInfoTablePrivate *d;

};
}

#endif // APPINFOTABLE_H
