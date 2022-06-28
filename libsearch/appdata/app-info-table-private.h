#ifndef APPINFOTABLEPRIVATE_H
#define APPINFOTABLEPRIVATE_H

#include "app-info-table.h"
#include <QObject>
#include <QDBusInterface>
#include <QSqlDatabase>

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

    //下面三个暂时未封装
    bool setAppLaunchTimes(QString &desktopfp, size_t num);
    bool updateAppLaunchTimes(QString &desktopfp);
    bool setAppLockState(QString &desktopfp, size_t num);

    //拖动改变置顶和收藏应用位置
    bool changeFavoriteAppPos(const QString &desktopfp, size_t pos);
    bool changeTopAppPos(const QString &desktopfp, size_t pos);

    bool getAllAppDesktopList(QStringList &list);
    bool getFavoritesAppList(QStringList &list);
    bool getTopAppList(QStringList &list);
    bool getLaunchTimesAppList(QStringList &list);
    bool getAppCategory(QString &desktopfp, QString &category);

    //获取所有应用信息并存到一个结构体中
    bool getAppInfoResults(QVector<AppInfoResult> &appInfoResults);

    //获取单个应用的某个状态（锁定，置顶，打开状态，收藏）
    bool getAppLockState(QString &desktopfp, size_t &num);
    bool getAppTopState(QString &desktopfp, size_t &num);
    bool getAppLaunchedState(QString &desktopfp, size_t &num);
    bool getAppFavoriteState(QString &desktopfp, size_t &num);

    //添加快捷方式
    bool addAppShortcut2Desktop(QString &desktopfp);
    bool addAppShortcut2Panel(QString &desktopfp);

    //搜索接口
    bool searchInstallApp(QString &keyWord, QStringList &installAppInfoRes);
    bool searchInstallApp(QStringList &keyWord, QStringList &installAppInfoRes);

    bool uninstallApp(QString &desktopfp);

    QString lastError(void) const;

private:
    ~AppInfoTablePrivate();
    bool initDateBaseConnection();
    bool openDataBase();
    void closeDataBase();

    QDBusInterface *m_interface = nullptr;

    AppInfoTable *q = nullptr;
    QSqlDatabase m_database;
    QString m_ConnectionName;

public Q_SLOTS:
    void sendAppDBItemsUpdate(QVector<AppInfoResult> results);
    void sendAppDBItemsAdd(QVector<AppInfoResult> results);
    void sendAppDBItemsDelete(QStringList desktopfps);

};


}
#endif // APPINFOTABLEPRIVATE_H
