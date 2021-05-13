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
 * Authors: zhangzihao <zhangzihao@kylinos.cn>
 * Modified by: zhangpengfei <zhangpengfei@kylinos.cn>
 *
 */
#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#include <QObject>
#include <QSettings>
#include <QMutex>
#include <QVector>
#include <QDir>
//#include <QGSettings>
//If use pkg_config, it wont build succes,why?????????
//My demo can build access yet.
//MouseZhangZh
#include <QGSettings/QGSettings>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QApplication>
#include "libsearch_global.h"

#define CONTROL_CENTER_PERSONALISE_GSETTINGS_ID "org.ukui.control-center.personalise"
#define TRANSPARENCY_KEY "transparency"
#define THEME_GSETTINGS_ID "org.ukui.style"
#define STYLE_NAME_KEY "styleName"
#define FONT_SIZE_KEY "systemFontSize"
#define ICON_THEME_KEY "iconThemeName"
#define INDEX_DATABASE_STATE "index_database_state"
#define CONTENT_INDEX_DATABASE_STATE "content_index_database_state"
#define INDEX_GENERATOR_NORMAL_EXIT "index_generator_normal_exit"
#define INOTIFY_NORMAL_EXIT "inotify_normal_exit"
#define ENABLE_CREATE_INDEX_ASK_DIALOG "enable_create_index_ask_dialog"
#define WEB_ENGINE "web_engine"
#define PATH_EMPTY 1;
#define PATH_NOT_IN_HOME 2;
#define PATH_PARENT_BLOCKED 3;

#define MAIN_SETTINGS QDir::homePath() + "/.config/org.ukui/ukui-search/ukui-search.conf"
#define BLOCK_DIRS QDir::homePath() + "/.config/org.ukui/ukui-search/ukui-search-block-dirs.conf"
#define SEARCH_HISTORY QDir::homePath() + "/.config/org.ukui/ukui-search/ukui-search-history.conf"
//#define CLOUD_HISTORY "history"
//#define CLOUD_APPLICATIONS "applications"

namespace Zeeker {
class LIBSEARCH_EXPORT GlobalSettings : public QObject {
    Q_OBJECT
public:
    static GlobalSettings *getInstance();
    const QVariant getValue(const QString&);
    bool isExist(const QString&);

Q_SIGNALS:
    void valueChanged(const QString&);
    void transparencyChanged(const double&);

public Q_SLOTS:
    void setValue(const QString&, const QVariant&);
    void reset(const QString&);
    void resetAll();
    /**
     * @brief setBlockDirs
     * set path for blacklist,return true if success,otherwise return false.
     * @param path path to be blocked
     * @param returnMessage this message will be set when return false.
     * @param true to remove blocking,false to set blocking,default set false.
     * @return
     */
    bool setBlockDirs(const QString& path, int &returnCode, bool remove = false);
    QStringList getBlockDirs();
//    void appendCloudData(const QString& key, const QString& value);
    void setSearchRecord(const QString &word, const QDateTime &time);
    QStringList getSearchRecord();
//    bool removeOneCloudData(const QString& key, const QString& value);
//    bool removeAllCloudData(const QString& key);
//    QStringList getCloudData(const QString& key);

    void forceSync(const QString& = nullptr);
    void updateSearchHistory(QString key);

private:
    explicit GlobalSettings(QObject *parent = nullptr);
    ~GlobalSettings() = default;

    QSettings* m_settings;
    QGSettings* m_trans_gsettings;
    QGSettings* m_theme_gsettings;
    QSettings *m_block_dirs_settings;
    QSettings *m_search_record_settings;
    QMap<QString, QVariant> m_cache;
    QStringList m_history;

    QMutex m_mutex;

};

}

#endif // GLOBALSETTINGS_H
