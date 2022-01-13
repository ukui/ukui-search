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
#include "app-match.h"
#include <glib.h>
#include <qt5xdg/XdgIcon>
#include "file-utils.h"
#include "app-search-plugin.h"
#define ANDROID_APP_DESKTOP_PATH QDir::homePath() + "/.local/share/applications/"
using namespace Zeeker;
static AppMatch *app_match_Class = nullptr;

AppMatch *AppMatch::getAppMatch() {
    if (!app_match_Class) {
        app_match_Class = new AppMatch;
    }
    return app_match_Class;
}
AppMatch::AppMatch(QObject *parent) : QThread(parent)
//    m_versionCommand(new QProcess(this))
{
    m_watchAppDir = new QFileSystemWatcher(this);
    m_watchAppDir->addPath("/usr/share/applications/");
    QDir androidPath(ANDROID_APP_DESKTOP_PATH);
    if (!androidPath.exists()) {
        androidPath.mkpath(ANDROID_APP_DESKTOP_PATH);
    }
    m_watchAppDir->addPath(ANDROID_APP_DESKTOP_PATH);
    qDBusRegisterMetaType<QMap<QString, QString>>();
    qDBusRegisterMetaType<QList<QMap<QString, QString>>>();
    m_interFace = new QDBusInterface("com.kylin.softwarecenter.getsearchresults", "/com/kylin/softwarecenter/getsearchresults",
                                     "com.kylin.getsearchresults",
                                     QDBusConnection::sessionBus());
    if (!m_interFace->isValid()) {
        qWarning() << qPrintable(QDBusConnection::sessionBus().lastError().message());
    }
    m_interFace->setTimeout(200);
    m_disabledAppSetting = new QSettings(QDir::homePath() + "/.cache/ukui-menu/ukui-menu.ini", QSettings::IniFormat, this);
    m_disabledAppSetting->setIniCodec(QTextCodec::codecForName("utf-8"));
    qDebug() << "AppMatch init finished.";
}

AppMatch::~AppMatch() {
    if (m_interFace) {
        delete m_interFace;
    }
    m_interFace = NULL;
    if (m_watchAppDir) {
        delete m_watchAppDir;
    }
    m_watchAppDir = NULL;
}

void AppMatch::startMatchApp(QString input, size_t uniqueSymbol, DataQueue<SearchPluginIface::ResultInfo> *searchResult) {
    m_disabledAppSetting->beginGroup("application");
    m_disabledAppSetting->sync();
    appNameMatch(input, uniqueSymbol, searchResult);
    m_disabledAppSetting->endGroup();
    slotDBusCallFinished(input, uniqueSymbol, searchResult);
    qDebug() << "App match finished!";
}

/**
 * @brief AppMatch::getAllDesktopFilePath 遍历所有desktop文件
 * @param path 存放desktop文件夹
 */
void AppMatch::getAllDesktopFilePath(QString path) {

    QDir dir(path);
    if (!dir.exists()) {
        return;
    }
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::DirsFirst);
    QFileInfoList list = dir.entryInfoList();
    list.removeAll(QFileInfo("/usr/share/applications/screensavers"));
    if (list.size() < 1) {
        return;
    }
    XdgDesktopFile desktopFile;
    int i = 0;
    while(i < list.size()) {
        QFileInfo fileInfo = list.at(i);
        //如果是文件夹，递归
        bool isDir = fileInfo.isDir();
        if (isDir) {
            getAllDesktopFilePath(fileInfo.filePath());
            qDebug() << fileInfo.filePath();
            ++i;
        } else {
            QString filePathStr = fileInfo.filePath();
            if (m_ExcludedDesktopfiles.contains(filePathStr)) {
                ++i;
                continue;
            }

            //过滤后缀不是.desktop的文件
            if (!filePathStr.endsWith(".desktop")) {
                ++i;
                continue;
            }

            desktopFile.load(filePathStr);
            if (desktopFile.value("NoDisplay").toString().contains("true") || desktopFile.value("NotShowIn").toString().contains("UKUI")) {
                ++i;
                continue;
            }

            QString name = desktopFile.localizedValue("Name").toString();
            if (name.isEmpty()) {
                ++i;
                qDebug() << filePathStr << "name!!";
                continue;
            }

            QString icon = desktopFile.iconName();
            NameString appname;
            QStringList appInfolist;

            appname.app_name = name;
            appInfolist << filePathStr << icon;
            appInfolist.append(desktopFile.value("Name").toString());
            appInfolist.append(desktopFile.value("Name[zh_CN]").toString());
            m_installAppMap.insert(appname, appInfolist);
            ++i;
        }
    }
}

/**
 * @brief AppMatch::appNameMatch
 * 进行匹配
 * @param appname
 * 应用名字
 */
void AppMatch::appNameMatch(QString keyWord, size_t uniqueSymbol, DataQueue<SearchPluginIface::ResultInfo> *searchResult) {
    QMapIterator<NameString, QStringList> iter(m_installAppMap);
    while(iter.hasNext()) {
        iter.next();
        if (iter.key().app_name.contains(keyWord, Qt::CaseInsensitive)) {
            SearchPluginIface::ResultInfo ri;
            creatResultInfo(ri, iter, true);
            AppSearchPlugin::m_mutex.lock();
            if (uniqueSymbol == AppSearchPlugin::uniqueSymbol) {
                searchResult->enqueue(ri);
                AppSearchPlugin::m_mutex.unlock();
                continue;
            } else {
                AppSearchPlugin::m_mutex.unlock();
                return;
            }
        }

        QStringList pinyinlist;
        pinyinlist = FileUtils::findMultiToneWords(iter.key().app_name);

        bool matched = false;
        for(int i = 0; i < pinyinlist.size() / 2; i++) {
            QString shouzimu = pinyinlist.at(2 * i + 1); // 中文转首字母
            if (shouzimu.contains(keyWord, Qt::CaseInsensitive)) {
                SearchPluginIface::ResultInfo ri;
                creatResultInfo(ri, iter, true);
                AppSearchPlugin::m_mutex.lock();
                if (uniqueSymbol == AppSearchPlugin::uniqueSymbol) {
                    searchResult->enqueue(ri);
                    AppSearchPlugin::m_mutex.unlock();
                    matched = true;
                    break;
                } else {
                    AppSearchPlugin::m_mutex.unlock();
                    return;
                }
            }
            if (keyWord.size() < 2)
                break;
            QString pinyin = pinyinlist.at(2 * i); // 中文转拼音
            if (pinyin.contains(keyWord, Qt::CaseInsensitive)) {
                SearchPluginIface::ResultInfo ri;
                AppSearchPlugin::m_mutex.lock();
                creatResultInfo(ri, iter, true);
                if (uniqueSymbol == AppSearchPlugin::uniqueSymbol) {
                    searchResult->enqueue(ri);
                    AppSearchPlugin::m_mutex.unlock();
                    matched = true;
                    break;
                } else {
                    AppSearchPlugin::m_mutex.unlock();
                    return;
                }
            }
        }
        if (matched) {
            continue;
        }
        QStringList tmpList;
        tmpList << iter.value().at(2) << iter.value().at(3);
        for(QString s : tmpList) {
            if (s.contains(keyWord, Qt::CaseInsensitive)) {
                SearchPluginIface::ResultInfo ri;
                AppSearchPlugin::m_mutex.lock();
                creatResultInfo(ri, iter, true);
                if (uniqueSymbol == AppSearchPlugin::uniqueSymbol) {
                    searchResult->enqueue(ri);
                    AppSearchPlugin::m_mutex.unlock();
                    break;
                } else {
                    AppSearchPlugin::m_mutex.unlock();
                    return;
                }
            }
        }
    }
}

void AppMatch::softWareCenterSearch(QMap<NameString, QStringList> &softwarereturn) {
//    if (m_interFace->timeout() != -1) {
//        qWarning() << "softWareCente Dbus is timeout !";
//        return;
//    }
//    slotDBusCallFinished(softwarereturn);
    qDebug() << "softWareCenter match app is successful!";
}

void AppMatch::slotDBusCallFinished(QString keyWord, size_t uniqueSymbol, DataQueue<SearchPluginIface::ResultInfo> *searchResult) {
    QDBusReply<QList<QMap<QString, QString>>> reply = m_interFace->call("get_search_result", keyWord); //阻塞，直到远程方法调用完成。
//    QDBusPendingReply<QList<QMap<QString,QString>>> reply = *call;
    if (reply.isValid()) {
        parseSoftWareCenterReturn(reply.value(), uniqueSymbol, searchResult);
    } else {
        qWarning() << "SoftWareCenter dbus called failed!";
    }
//     call->deleteLater();
}

void AppMatch::parseSoftWareCenterReturn(QList<QMap<QString, QString>> list, size_t uniqueSymbol, DataQueue<SearchPluginIface::ResultInfo> *searchResult) {
    qDebug() << "Begin parseSoftWareCenterReturn";
    QLocale locale;
    for(int i = 0; i < list.size(); i++) {
        SearchPluginIface::ResultInfo ri;
        if (locale.language() == QLocale::Chinese) {
            ri.name = list.at(i).value("displayname_cn");
        } else {
            ri.name = list.at(i).value("appname");
        }
        ri.icon = !(QIcon(list.at(i).value("icon")).isNull()) ? QIcon(list.at(i).value("icon")) : QIcon(":/res/icons/desktop.png");
        SearchPluginIface::DescriptionInfo di;
        di.key = QString(tr("Application Description:"));
        di.value = list.at(i).value("discription");
        ri.description.append(di);
        ri.actionKey = list.at(i).value("appname");
        ri.type = 1; //1 means not installed apps.
        AppSearchPlugin::m_mutex.lock();
        if (uniqueSymbol == AppSearchPlugin::uniqueSymbol) {
            searchResult->enqueue(ri);
            AppSearchPlugin::m_mutex.unlock();
        } else {
            AppSearchPlugin::m_mutex.unlock();
            break;
        }
    }
}

void AppMatch::creatResultInfo(SearchPluginIface::ResultInfo &ri, QMapIterator<NameString, QStringList> &iter, bool isInstalled)
{
    m_desktopFile.load(iter.value().at(0));
    ri.icon = XdgIcon::fromTheme(iter.value().at(1), QIcon(":/res/icons/desktop.png"));
    if (!m_disabledAppSetting->value(m_desktopFile.value("Exec").toString(), true).toBool()) {
        ri.icon = QIcon(ri.icon.pixmap(240, 240, QIcon::Disabled));
    }
    ri.name = iter.key().app_name;
    ri.actionKey = iter.value().at(0);
    ri.type = 0; //0 means installed apps.
}

void AppMatch::run() {
    qDebug() << "App map init..";
    this->getAllDesktopFilePath("/usr/share/applications/");
    this->getAllDesktopFilePath(ANDROID_APP_DESKTOP_PATH);
    connect(m_watchAppDir, &QFileSystemWatcher::directoryChanged, this, [ = ](const QString & path) {
            m_installAppMap.clear();
            this->getAllDesktopFilePath("/usr/share/applications/");
            this->getAllDesktopFilePath(ANDROID_APP_DESKTOP_PATH);
            qDebug() << "App map update " << m_installAppMap.size();
    });
    qDebug() << "App map init finished.." << m_installAppMap.size();
}
