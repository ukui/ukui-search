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
class AppMatch : public QObject
{
    Q_OBJECT
public:
    explicit AppMatch(QObject *parent = nullptr);
    QStringList startMatchApp(QString input);

private:
    void getAllDesktopFilePath(QString path);
    void getDesktopFilePath();
    void getAppName();
    void appNameMatch(QString appname,QString desktoppath);

private:
    QString m_sourceText;
    QStringList m_filePathList;
    QStringList m_returnResult;
    QStringList m_midResult;

};

#endif // APPMATCH_H
