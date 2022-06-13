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
#include <qt5xdg/XdgDesktopFile>
#include <QMap>
#include "file-utils.h"
#include "app-search-plugin.h"

using namespace UkuiSearch;
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
    qDBusRegisterMetaType<QMap<QString, QString>>();
    qDBusRegisterMetaType<QList<QMap<QString, QString>>>();
    m_interFace = new QDBusInterface("com.kylin.softwarecenter.getsearchresults", "/com/kylin/softwarecenter/getsearchresults",
                                     "com.kylin.getsearchresults",
                                     QDBusConnection::sessionBus());
    if(!m_interFace->isValid()) {
        qWarning() << qPrintable(QDBusConnection::sessionBus().lastError().message());
    }
    m_interFace->setTimeout(1500);
    m_appInfoTable = new AppInfoTable;
    qDebug() << "AppMatch init finished.";
}

AppMatch::~AppMatch() {
    if(m_interFace) {
        delete m_interFace;
    }
    m_interFace = NULL;
    if(m_appInfoTable) {
        delete m_appInfoTable;
    }
    m_appInfoTable = NULL;
}

void AppMatch::startMatchApp(QString input, size_t uniqueSymbol, DataQueue<SearchPluginIface::ResultInfo> *searchResult) {
    appNameMatch(input, uniqueSymbol, searchResult);
    slotDBusCallFinished(input, uniqueSymbol, searchResult);
    qDebug() << "App match finished!";
}

/**
 * @brief AppMatch::appNameMatch
 * 进行匹配
 * @param appname
 * 应用名字
 */
void AppMatch::appNameMatch(QString keyWord, size_t uniqueSymbol, DataQueue<SearchPluginIface::ResultInfo> *searchResult) {
    QStringList results;
    //m_appInfoTable->searchInstallAppOrderByFavoritesDesc(keyWord, results);
    for (int i = 0; i < results.size() / 3; i++) {
        {
            QMutexLocker locker(&AppSearchPlugin::m_mutex);
            if (uniqueSymbol != AppSearchPlugin::uniqueSymbol) {
                return;
            }
        }

        SearchPluginIface::ResultInfo ri;
        ri.actionKey = results.at(i*3);
        ri.name = results.at(i*3 + 1);
        ri.icon = XdgIcon::fromTheme(results.at(i*3 + 2), QIcon(":/res/icons/desktop.png"));
        ri.type = 0;

        searchResult->enqueue(ri);
    }

/*    QMultiMap<QString, QStringList> installAppMap;
    m_appInfoTable->getInstallAppMap(installAppMap);
    QMap<NameString, QStringList> resultAppMap;
    for (auto i = installAppMap.begin(); i != installAppMap.end(); ++i) {
        NameString name;
        name.app_name = i.key();
        QStringList infoList;
        infoList = i.value();
        resultAppMap.insert(name, infoList);
    }
    QMapIterator<NameString, QStringList> iter(resultAppMap);
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

        if(iter.value().at(3) == ""){
            continue;
        }
        QStringList pinyinlist;
        pinyinlist = FileUtils::findMultiToneWords(iter.value().at(3));

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
    }*/
}

void AppMatch::slotDBusCallFinished(QString keyWord, size_t uniqueSymbol, DataQueue<SearchPluginIface::ResultInfo> *searchResult) {
    QDBusReply<QList<QMap<QString, QString>>> reply = m_interFace->call("get_search_result", keyWord); //阻塞，直到远程方法调用完成。
    if(reply.isValid()) {
        parseSoftWareCenterReturn(reply.value(), uniqueSymbol, searchResult);
    } else {
        qWarning() << "SoftWareCenter dbus called failed!";
    }
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

//void AppMatch::creatResultInfo(SearchPluginIface::ResultInfo &ri, QMapIterator<NameString, QStringList> &iter, bool isInstalled)
//{
////    ri.icon = QIcon::fromTheme(iter.value().at(1), QIcon(":/res/icons/desktop.png"));
//    ri.icon = XdgIcon::fromTheme(iter.value().at(1), QIcon(":/res/icons/desktop.png"));
//    ri.name = iter.key().app_name;
//    ri.actionKey = iter.value().at(0);
//    ri.type = 0; //0 means installed apps.
//}

void AppMatch::run() {
    qDebug() << "App map init..";

    qDebug() << "App map init finished..";
}
