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
    if(!app_match_Class) {
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
    if(!androidPath.exists()) {
        androidPath.mkpath(ANDROID_APP_DESKTOP_PATH);
    }
    m_watchAppDir->addPath(ANDROID_APP_DESKTOP_PATH);
    qDBusRegisterMetaType<QMap<QString, QString>>();
    qDBusRegisterMetaType<QList<QMap<QString, QString>>>();
    m_interFace = new QDBusInterface("com.kylin.softwarecenter.getsearchresults", "/com/kylin/softwarecenter/getsearchresults",
                                     "com.kylin.getsearchresults",
                                     QDBusConnection::sessionBus());
    if(!m_interFace->isValid()) {
        qWarning() << qPrintable(QDBusConnection::sessionBus().lastError().message());
    }
    m_interFace->setTimeout(200);
    qDebug() << "AppMatch init finished.";
}

AppMatch::~AppMatch() {
    if(m_interFace) {
        delete m_interFace;
    }
    m_interFace = NULL;
    if(m_watchAppDir) {
        delete m_watchAppDir;
    }
    m_watchAppDir = NULL;
}

void AppMatch::startMatchApp(QString input, size_t uniqueSymbol, DataQueue<SearchPluginIface::ResultInfo> *searchResult) {
    appNameMatch(input, uniqueSymbol, searchResult);
    slotDBusCallFinished(input, uniqueSymbol, searchResult);
    qDebug() << "App match finished!";
}

/**
 * @brief AppMatch::getAllDesktopFilePath 遍历所有desktop文件
 * @param path 存放desktop文件夹
 */
void AppMatch::getAllDesktopFilePath(QString path) {

    char* name;
    char* icon;

    GKeyFileFlags flags = G_KEY_FILE_NONE;
    GKeyFile* keyfile = g_key_file_new();

    QDir dir(path);
    if(!dir.exists()) {
        return;
    }
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::DirsFirst);
    QFileInfoList list = dir.entryInfoList();
    list.removeAll(QFileInfo("/usr/share/applications/screensavers"));
    if(list.size() < 1) {
        return;
    }
    int i = 0;
    //递归算法的核心部分
    do {
        QFileInfo fileInfo = list.at(i);
        //如果是文件夹，递归
        bool isDir = fileInfo.isDir();
        if(isDir) {
            getAllDesktopFilePath(fileInfo.filePath());
        } else {
            //过滤LXQt、KDE
            QString filePathStr = fileInfo.filePath();
//            if(filePathStr.contains("KDE", Qt::CaseInsensitive) ||
//               filePathStr.contains("mate",Qt::CaseInsensitive)||
//                    filePathStr.contains("LX", Qt::CaseInsensitive)) {
//                i++;
//                continue;
//            }
            //过滤后缀不是.desktop的文件
            if(!filePathStr.endsWith(".desktop")) {
                i++;
                continue;
            }
            QByteArray fpbyte = filePathStr.toLocal8Bit();
            char* filepath = fpbyte.data();
            g_key_file_load_from_file(keyfile, filepath, flags, nullptr);
            char* ret_1 = g_key_file_get_locale_string(keyfile, "Desktop Entry", "NoDisplay", nullptr, nullptr);
            if(ret_1 != nullptr) {
                QString str = QString::fromLocal8Bit(ret_1);
                if(str.contains("true")) {
                    i++;
                    continue;
                }
            }
            char* ret_2 = g_key_file_get_locale_string(keyfile, "Desktop Entry", "NotShowIn", nullptr, nullptr);
            if(ret_2 != nullptr) {
                QString str = QString::fromLocal8Bit(ret_2);
                if(str.contains("UKUI")) {
                    i++;
                    continue;
                }
            }
            //过滤中英文名为空的情况
            QLocale cn;
            QString language = cn.languageToString(cn.language());
            if(QString::compare(language, "Chinese") == 0) {
                char* nameCh = g_key_file_get_string(keyfile, "Desktop Entry", "Name[zh_CN]", nullptr);
                char* nameEn = g_key_file_get_string(keyfile, "Desktop Entry", "Name", nullptr);
                if(QString::fromLocal8Bit(nameCh).isEmpty() && QString::fromLocal8Bit(nameEn).isEmpty()) {
                    i++;
                    continue;
                }
            } else {
                char* name = g_key_file_get_string(keyfile, "Desktop Entry", "Name", nullptr);
                if(QString::fromLocal8Bit(name).isEmpty()) {
                    i++;
                    continue;
                }
            }
            name = g_key_file_get_locale_string(keyfile, "Desktop Entry", "Name", nullptr, nullptr);
            icon = g_key_file_get_locale_string(keyfile, "Desktop Entry", "Icon", nullptr, nullptr);
            if(!m_filePathList.contains(filePathStr)) {
                NameString appname;
                QStringList appInfolist;

                appname.app_name = QString::fromLocal8Bit(name);
                appInfolist << filePathStr << QString::fromLocal8Bit(icon);
                appInfolist.append(QString::fromLocal8Bit(g_key_file_get_string(keyfile, "Desktop Entry", "Name", nullptr)));
                appInfolist.append(QString::fromLocal8Bit(g_key_file_get_string(keyfile, "Desktop Entry", "Name[zh_CN]", nullptr)));

                m_installAppMap.insert(appname, appInfolist);
            }
//            m_filePathList.append(filePathStr);
        }
        i++;

    } while(i < list.size());
    g_key_file_free(keyfile);
}

/**
 * @brief AppMatch::getDesktopFilePath
 * 对遍历结果处理
 */
void AppMatch::getDesktopFilePath() {
    m_filePathList.clear();
    m_installAppMap.clear();
    m_filePathList.append("/usr/share/applications/software-properties-livepatch.desktop");
    m_filePathList.append("/usr/share/applications/mate-color-select.desktop");
    m_filePathList.append("/usr/share/applications/blueman-adapters.desktop");
    m_filePathList.append("/usr/share/applications/blueman-manager.desktop");
    m_filePathList.append("/usr/share/applications/mate-user-guide.desktop");
    m_filePathList.append("/usr/share/applications/nm-connection-editor.desktop");
    m_filePathList.append("/usr/share/applications/debian-uxterm.desktop");
    m_filePathList.append("/usr/share/applications/debian-xterm.desktop");
    m_filePathList.append("/usr/share/applications/im-config.desktop");
    m_filePathList.append("/usr/share/applications/fcitx.desktop");
    m_filePathList.append("/usr/share/applications/fcitx-configtool.desktop");
    m_filePathList.append("/usr/share/applications/onboard-settings.desktop");
    m_filePathList.append("/usr/share/applications/info.desktop");
    m_filePathList.append("/usr/share/applications/ukui-power-preferences.desktop");
    m_filePathList.append("/usr/share/applications/ukui-power-statistics.desktop");
    m_filePathList.append("/usr/share/applications/software-properties-drivers.desktop");
    m_filePathList.append("/usr/share/applications/software-properties-gtk.desktop");
    m_filePathList.append("/usr/share/applications/gnome-session-properties.desktop");
    m_filePathList.append("/usr/share/applications/org.gnome.font-viewer.desktop");
    m_filePathList.append("/usr/share/applications/xdiagnose.desktop");
    m_filePathList.append("/usr/share/applications/gnome-language-selector.desktop");
    m_filePathList.append("/usr/share/applications/mate-notification-properties.desktop");
    m_filePathList.append("/usr/share/applications/transmission-gtk.desktop");
    m_filePathList.append("/usr/share/applications/mpv.desktop");
    m_filePathList.append("/usr/share/applications/system-config-printer.desktop");
    m_filePathList.append("/usr/share/applications/org.gnome.DejaDup.desktop");
    m_filePathList.append("/usr/share/applications/yelp.desktop");
    m_filePathList.append("/usr/share/applications/peony-computer.desktop");
    m_filePathList.append("/usr/share/applications/peony-home.desktop");
    m_filePathList.append("/usr/share/applications/peony-trash.desktop");
//    m_filePathList.append("/usr/share/applications/peony.desktop");

    //v10
    m_filePathList.append("/usr/share/applications/mate-about.desktop");
    m_filePathList.append("/usr/share/applications/time.desktop");
    m_filePathList.append("/usr/share/applications/network.desktop");
    m_filePathList.append("/usr/share/applications/shares.desktop");
    m_filePathList.append("/usr/share/applications/mate-power-statistics.desktop");
    m_filePathList.append("/usr/share/applications/display-im6.desktop");
    m_filePathList.append("/usr/share/applications/display-im6.q16.desktop");
    m_filePathList.append("/usr/share/applications/openjdk-8-policytool.desktop");
    m_filePathList.append("/usr/share/applications/kylin-io-monitor.desktop");
    m_filePathList.append("/usr/share/applications/wps-office-uninstall.desktop");

//    QString desktop;
//    QStringList applist;
//    QMap<QString, QList<QString>>::const_iterator i;
//    for(i=m_installAppMap.constBegin();i!=m_installAppMap.constEnd();++i){
//        applist=i.value();
//        if(m_filePathList.contains(applist.at(0)))
//            m_filterInstallAppMap.insert(i.key(),applist);
//    }
}

void AppMatch::getAppName(QMap<NameString, QStringList> &installed) {
//    QMap<NameString, QStringList>::const_iterator i;
//    for(i = m_installAppMap.constBegin(); i != m_installAppMap.constEnd(); ++i) {
//        appNameMatch(i.key().app_name, installed);
//    }
//    appNameMatch(installed);
//    qDebug() << "installed app match is successful!";
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
        if(iter.key().app_name.contains(keyWord, Qt::CaseInsensitive)) {
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
            if(shouzimu.contains(keyWord, Qt::CaseInsensitive)) {
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
            if(keyWord.size() < 2)
                break;
            QString pinyin = pinyinlist.at(2 * i); // 中文转拼音
            if(pinyin.contains(keyWord, Qt::CaseInsensitive)) {
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
        if(matched) {
            continue;
        }
        QStringList tmpList;
        tmpList << iter.value().at(2) << iter.value().at(3);
        for(QString s : tmpList) {
            if(s.contains(keyWord, Qt::CaseInsensitive)) {
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
//    if(m_interFace->timeout() != -1) {
//        qWarning() << "softWareCente Dbus is timeout !";
//        return;
//    }
//    slotDBusCallFinished(softwarereturn);
    qDebug() << "softWareCenter match app is successful!";
}

void AppMatch::slotDBusCallFinished(QString keyWord, size_t uniqueSymbol, DataQueue<SearchPluginIface::ResultInfo> *searchResult) {
    QDBusReply<QList<QMap<QString, QString>>> reply = m_interFace->call("get_search_result", keyWord); //阻塞，直到远程方法调用完成。
//    QDBusPendingReply<QList<QMap<QString,QString>>> reply = *call;
    if(reply.isValid()) {
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
        if(locale.language() == QLocale::Chinese) {
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

void AppMatch::getInstalledAppsVersion(QString appname) {
//    qWarning()<<"apt show "+appname;
//    m_versionCommand->start("apt show "+appname);
//    m_versionCommand->startDetached(m_versionCommand->program());
//    m_versionCommand->waitForFinished();
//    connect(m_versionCommand,&QProcess::readyReadStandardOutput,this,[=](){
//        QString result=m_versionCommand->readAllStandardOutput();
//        if(!result.isEmpty()){
//            QStringList strlist=result.split("\n");
//            QString str=strlist.at(1);
//            if(!str.contains("Version")){
//               return;
//            }
//            qWarning()<<strlist.at(1);
//            qWarning()<<"-----------------------------------------------";
//        }
//    });
    //    m_versionCommand->close();
}

void AppMatch::creatResultInfo(SearchPluginIface::ResultInfo &ri, QMapIterator<NameString, QStringList> &iter, bool isInstalled)
{
//    ri.icon = QIcon::fromTheme(iter.value().at(1), QIcon(":/res/icons/desktop.png"));
    ri.icon = XdgIcon::fromTheme(iter.value().at(1), QIcon(":/res/icons/desktop.png"));
    ri.name = iter.key().app_name;
    ri.actionKey = iter.value().at(0);
    ri.type = 0; //0 means installed apps.
}

void AppMatch::run() {
    qDebug() << "AppMatch is run";
    this->getDesktopFilePath();
    this->getAllDesktopFilePath("/usr/share/applications/");
    this->getAllDesktopFilePath(ANDROID_APP_DESKTOP_PATH);
    connect(m_watchAppDir, &QFileSystemWatcher::directoryChanged, this, [ = ](const QString & path) {
        if(path == "/usr/share/applications/") {
            this->getAllDesktopFilePath("/usr/share/applications/");
        } else if(path == ANDROID_APP_DESKTOP_PATH) {
            this->getAllDesktopFilePath(ANDROID_APP_DESKTOP_PATH);
        }
    });
}
