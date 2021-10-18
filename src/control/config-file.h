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
#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <QObject>
#include <QSettings>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#define HOMEPAGE_SETTINGS  "media/用户保险箱/.ukui-search/ukui-search-homepage.conf"
namespace Zeeker {
class ConfigFile : public QObject {
    Q_OBJECT
public:
    static ConfigFile *getInstance();
    bool writeConfig(QString message);
    QMap<QString, QStringList> readConfig();
    void removeConfig(QString path);
    bool itemExist(QString path);
    void receiveMessage(QString message);
private:
    explicit ConfigFile();
    ~ConfigFile();
    QStringList readCommonly();
    bool writeCommonly(QString message);
    void removeCommonly(QString path);

    QStringList readRecently();
    bool writeRecently(QString message);
    void removeRecently(QString path);
private:
    QSettings *m_settings;

};
}

#endif // CONFIGFILE_H
