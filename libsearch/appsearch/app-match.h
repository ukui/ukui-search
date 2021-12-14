/*
 * Copyright (C) 2020, KylinSoft Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: sunfengsheng <sunfengsheng@kylinos.cn>
 *
 */
#ifndef APPMATCH_H
#define APPMATCH_H

#include <QObject>
#include <QDir>
#include <QLocale>
#include <QDebug>
#include <QDBusInterface>
#include <QDBusReply>
#include <QtDBus>
#include <QThread>
#include "search-plugin-iface.h"
namespace UkuiSearch {
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

//struct NameString
//{
//    QString app_name;
//    //重载操作符
//    inline bool operator < (const NameString& name) const
//    {
////        return  name.app_name.length() >= app_name.length();
//        return true;
//    }
//};

class AppMatch : public QThread {
    Q_OBJECT
public:
    static AppMatch *getAppMatch();
    void startMatchApp(QString input, size_t uniqueSymbol, DataQueue<SearchPluginIface::ResultInfo> *searchResult);

protected:
    void run() override;

private:
    explicit AppMatch(QObject *parent = nullptr);
    ~AppMatch();
    void getAllDesktopFilePath(QString path);
    void appNameMatch(QString keyWord, size_t uniqueSymbol, DataQueue<SearchPluginIface::ResultInfo> *searchResult);
    void softWareCenterSearch(QMap<NameString, QStringList> &softwarereturn);
    void parseSoftWareCenterReturn(QList<QMap<QString, QString>> list, size_t uniqueSymbol, DataQueue<SearchPluginIface::ResultInfo> *searchResult);
    void creatResultInfo(SearchPluginIface::ResultInfo &ri, QMapIterator<NameString, QStringList> &iter, bool isInstalled = true);

    QString m_sourceText;
    size_t m_uniqueSymbol;
    DataQueue<SearchPluginIface::ResultInfo> *m_search_result = nullptr;
    QDBusInterface *m_interFace = nullptr;
    QFileSystemWatcher *m_watchAppDir = nullptr;
    QMap<NameString, QStringList> m_installAppMap;

    QStringList m_ExcludedDesktopfiles = {
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

private Q_SLOTS:
    void slotDBusCallFinished(QString keyWord, size_t uniqueSymbol, DataQueue<SearchPluginIface::ResultInfo> *searchResult);
};
}

#endif // APPMATCH_H
