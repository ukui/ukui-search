#ifndef APPDBMANAGER_H
#define APPDBMANAGER_H

#include "app-db-common.h"
#include "pending-app-info-queue.h"

#include <QDir>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QApplication>
#include <QDateTime>
#include <QFileSystemWatcher>
#include <QMutex>
#include <QSettings>
#include <QTimer>
#include <QThread>

#define CONNECTION_NAME QLatin1String("ukss-appdb-connection")

namespace UkuiSearch {
/**
 * @brief The AppDBManager class
 * 功能：1.遍历并且监听desktop文件目录，建立并且维护应用信息数据库。
 *      2.监听应用安装，打开事件，收藏等事件，更新数据库
 */
class NameString {
public:
    explicit NameString(const QString &str_) : app_name(str_) {}
    NameString() = default;
    QString app_name;
    bool operator<(const NameString& name) const {
        return this->app_name.length() <= name.app_name.length();
    }
    bool operator==(const NameString& name) const {
        return this->app_name == name.app_name;
    }
};

class AppDBManager : public QThread
{
    Q_OBJECT

    Q_CLASSINFO("D-Bus Interface","org.ukui.search.appDBManager")

    enum APP_LOCK_STATE{
       APP_UNLOCK = 0,
       APP_LOCK
    };

public:
    static AppDBManager *getInstance();

    //刷新数据库数据
    void refreshAllData2DB();

    //获取desktop文件的md5值
    QString getAppDesktopMd5(const QString &desktopfd);

    bool startTransaction();
    bool startCommit();

    bool handleDBItemInsert(const QString &desktopfd);
    bool handleDBItemUpdate(const QString &desktopfd);
    bool handleDBItemDelete(const QString &desktopfd);

    bool handleLocaleDataUpdate(const QString &desktopPath);
    bool handleLaunchTimesUpdate(const QString &desktopfp, int num);
    bool handleFavoritesStateUpdate(const QString &desktopfp, int num);
    bool handleTopStateUpdate(const QString &desktopfp, int num);
    bool handleLockStateUpdate(const QString &desktopfp, int num);

public Q_SLOTS:
    //对数据库单条所有信息进行增删改
    void insertDBItem(const QString &desktopfd);
    void updateDBItem(const QString &desktopfd);
    void deleteDBItem(const QString &desktopfd);

    //对数据库某字段进行update
    void updateLocaleData(const QString &desktopfp);
    void updateLaunchTimes(const QString &desktopfp);
    void updateFavoritesState(const QString &desktopfp, int num);
    void updateTopState(const QString &desktopfp, int num);
    void udpateLockState(const QString &desktopfp, int num);

    //拖动改变置顶和收藏应用位置
    bool changeFavoriteAppPos(const QString &desktopfp, int pos);
    bool changeTopAppPos(const QString &desktopfp, int pos);

    //获取数据库中全部信息
    QVector<AppInfoResult> getAppInfoResults();

    //查询某应用的某个字段的值
    int getAppLockState(const QString &desktopfp);
    int getAppTopState(const QString &desktopfp);
    int getAppLaunchedState(const QString &desktopfp);
    int getAppFavoriteState(const QString &desktopfp);
    QString getAppCategory(const QString &desktopfp);

protected:
    void run() override;

private:
    explicit AppDBManager(QObject *parent = nullptr);
    ~AppDBManager();

    //加载指定路径path中的所有desktop文件路径到infolist中
    void loadDesktopFilePaths(QString path, QFileInfoList &infolist);

    //链接数据库
    bool openDataBase();
    //刷新数据库
    void refreshDataBase();
    //关闭数据库，断开链接
    void closeDataBase();

    //创建数据库字段
    void buildAppInfoDB();

    //暂时弃用
    void updateAppInfoDB();
    void getAllDesktopFilePath(QString path);
    void getFilePathList(QStringList &pathList);
    void getInstallAppMap(QMap<QString, QStringList> &installAppMap);

private:
    static QMutex s_mutex;
    bool m_localeChanged;
    bool m_dbChanged;

    QSettings *m_qSettings = nullptr;

    QTimer *m_timer = nullptr;
    QTimer *m_maxProcessTimer = nullptr;

    QSqlDatabase m_database;
    QFileSystemWatcher *m_watchAppDir = nullptr;

    //应用黑名单
    QStringList m_excludedDesktopfiles = {
        "/usr/share/applications/software-properties-livepatch.desktop",
        "/usr/share/applications/mate-color-select.desktop",
        "/usr/share/applications/blueman-adapters.desktop",
        "/usr/share/applications/blueman-manager.desktop",
        "/usr/share/applications/mate-user-guide.desktop",
        "/usr/share/applications/nm-connection-editor.desktop",
        "/usr/share/applications/debian-uxterm.desktop",
        "/usr/share/applications/debian-xterm.desktop",
        "/usr/share/applications/im-config.desktop",
        "/usr/share/applications/fcitx.desktop",
        "/usr/share/applications/fcitx-configtool.desktop",
        "/usr/share/applications/onboard-settings.desktop",
        "/usr/share/applications/info.desktop",
        "/usr/share/applications/ukui-power-preferences.desktop",
        "/usr/share/applications/ukui-power-statistics.desktop",
        "/usr/share/applications/software-properties-drivers.desktop",
        "/usr/share/applications/software-properties-gtk.desktop",
        "/usr/share/applications/gnome-session-properties.desktop",
        "/usr/share/applications/org.gnome.font-viewer.desktop",
        "/usr/share/applications/xdiagnose.desktop",
        "/usr/share/applications/gnome-language-selector.desktop",
        "/usr/share/applications/mate-notification-properties.desktop",
        "/usr/share/applications/transmission-gtk.desktop",
        "/usr/share/applications/mpv.desktop",
        "/usr/share/applications/system-config-printer.desktop",
        "/usr/share/applications/org.gnome.DejaDup.desktop",
        "/usr/share/applications/yelp.desktop",
        "/usr/share/applications/peony-computer.desktop",
        "/usr/share/applications/peony-home.desktop",
        "/usr/share/applications/peony-trash.desktop",

        //v10
        "/usr/share/applications/mate-about.desktop",
        "/usr/share/applications/time.desktop",
        "/usr/share/applications/network.desktop",
        "/usr/share/applications/shares.desktop",
        "/usr/share/applications/mate-power-statistics.desktop",
        "/usr/share/applications/display-im6.desktop",
        "/usr/share/applications/display-im6.q16.desktop",
        "/usr/share/applications/openjdk-8-policytool.desktop",
        "/usr/share/applications/kylin-io-monitor.desktop",
        "/usr/share/applications/wps-office-uninstall.desktop",

        //原本额外排除的目录，不知道额外的原因，有可能之后有问题--bjj20220621
        "/usr/share/applications/screensavers"
    };

    //暂时弃用
    QMap<NameString, QStringList> m_installAppMap;

Q_SIGNALS:
    //操作数据库
    void appDBItemUpdate(const AppInfoResult&);
    void appDBItemAdd(const AppInfoResult&);
    void appDBItemDelete(const QString&);
    void finishHandleAppDB();

    //定时器操作
    void startTimer();
    void maxProcessTimerStart();
    void stopTimer();
};
}

#endif // APPDBMANAGER_H
