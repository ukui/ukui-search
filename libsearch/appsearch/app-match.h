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
#include <QElapsedTimer>
#include <QThread>

class AppMatch : public QThread
{
    Q_OBJECT
public:
    static AppMatch *getAppMatch();
    explicit AppMatch(QObject *parent = nullptr);
    ~AppMatch();
    void startMatchApp(QString input,QMap<QString,QStringList> &installed,QMap<QString,QStringList> &softwarereturn);
    QMap<QString,QList<QString>> startMatchApp(QString input);

private:
    void getAllDesktopFilePath(QString path);
    void getDesktopFilePath();
    void getAppName();
    void getAppName(QMap<QString,QStringList> &installed);
//    void appNameMatch(QString appname,QString desktoppath,QString appicon);
    void appNameMatch(QString appname);
    void appNameMatch(QString appname,QMap<QString,QStringList> &installed);

    void softWareCenterSearch();
    void softWareCenterSearch(QMap<QString,QStringList> &softwarereturn);

    void parseSoftWareCenterReturn(QList<QMap<QString,QString>> list);
    void parseSoftWareCenterReturn(QList<QMap<QString,QString>> list,QMap<QString,QStringList> &softwarereturn);

    void getInstalledAppsVersion(QString appname);
    void returnAppMap();

private:
    QString m_sourceText;
    QStringList m_filePathList;
    QStringList m_returnResult;

    QDBusInterface *m_interFace=nullptr;
    QFileSystemWatcher *m_watchAppDir=nullptr;
    QMap<QString,QList<QString>> m_softWareCenterMap;
    QMap<QString,QList<QString>> m_installAppMap;
    QMap<QString,QList<QString>> m_matchInstallAppMap;
    QMap<QString,QList<QString>> m_returnResult1;
    QMap<QString,QList<QString>> m_midResult;
//    QProcess *m_versionCommand;

private Q_SLOTS:
    void slotDBusCallFinished();
    void slotDBusCallFinished(QMap<QString,QStringList> &softwarereturn);

//Q_SIGNALS:

protected:
    void run() override;

};

#endif // APPMATCH_H
