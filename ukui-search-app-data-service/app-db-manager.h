#ifndef APPDBMANAGER_H
#define APPDBMANAGER_H

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
#include "app-db-common-defines.h"

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

class AppDBManager : public QObject
{
    Q_OBJECT

    enum APP_LOCK_STATE{
       APP_UNLOCK = 0,
       APP_LOCK
    };

public:
    static AppDBManager *getInstance();

    QString getAppDesktopMd5(QString &desktopfd);

    void getInstallAppMap(QMap<QString, QStringList> &installAppMap);

    bool addAppDesktopFile2DB(QString &desktopfd);
    bool deleteAppDesktopFile2DB(QString &desktopfd);
    bool updateAppDesktopFile2DB(QString &desktopfd);
    bool updateAppLaunchTimes(QString &desktopfp);

private:
    explicit AppDBManager(QObject *parent = nullptr);
    ~AppDBManager();

    void getAllDesktopFilePath(QString path);

    void initDateBaseConnection();
    void openDataBase();
    void closeDataBase();

    void buildAppInfoDB();
    void updateAppInfoDB();
    void getFilePathList(QStringList &pathList);

    QSqlDatabase *m_database = nullptr;

    QFileSystemWatcher *m_watchAppDir = nullptr;

    static QMutex s_installAppMapMutex;
    QMap<NameString, QStringList> m_installAppMap;

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
    };

};
}

#endif // APPDBMANAGER_H
