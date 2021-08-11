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
namespace Zeeker {
class NameString {
public:
    explicit NameString(const QString &str_) : app_name(str_) {}
    NameString() = default;
    QString app_name;
    bool operator<(const NameString& name) const {
        return this->app_name.length() <= name.app_name.length();
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
    void getDesktopFilePath();
    void getAppName(QMap<NameString, QStringList> &installed);
//    void appNameMatch(QString appname,QString desktoppath,QString appicon);
//    void appNameMatch(QString keyWord, QString appname, QMap<NameString, QStringList> &installed);
    void appNameMatch(QString keyWord, size_t uniqueSymbol, DataQueue<SearchPluginIface::ResultInfo> *searchResult);
    void softWareCenterSearch(QMap<NameString, QStringList> &softwarereturn);
    void parseSoftWareCenterReturn(QList<QMap<QString, QString>> list, size_t uniqueSymbol, DataQueue<SearchPluginIface::ResultInfo> *searchResult);
    void getInstalledAppsVersion(QString appname);
    void creatResultInfo(SearchPluginIface::ResultInfo &ri, QMapIterator<NameString, QStringList> &iter, bool isInstalled = true);

    QString m_sourceText;
    size_t m_uniqueSymbol;
    DataQueue<SearchPluginIface::ResultInfo> *m_search_result = nullptr;
    QStringList m_filePathList;

    QDBusInterface *m_interFace = nullptr;
    QFileSystemWatcher *m_watchAppDir = nullptr;
    QMap<NameString, QStringList> m_installAppMap;

private Q_SLOTS:
    void slotDBusCallFinished(QString keyWord, size_t uniqueSymbol, DataQueue<SearchPluginIface::ResultInfo> *searchResult);

//Q_SIGNALS:



};
}

#endif // APPMATCH_H
