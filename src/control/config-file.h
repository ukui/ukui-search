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
class ConfigFile : public QObject
{
    Q_OBJECT
public:
    static bool writeConfig(QString message);
    static QMap<QString,QStringList> readConfig();
    static void receiveMessage(QString message);
private:
   static bool writeCommonly(QString message);
   static QStringList readCommonly();
   static bool writeRecently(QString message);
   static QStringList readRecently();

};

#endif // CONFIGFILE_H
