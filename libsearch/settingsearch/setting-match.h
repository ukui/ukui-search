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
#ifndef SETTINGSEARCH_H
#define SETTINGSEARCH_H

#include <QObject>
#include <QFile>
#include <QDomElement>
#include <QDomDocument>
#include <QDomNode>
#include <QDomNodeList>
#include <QMap>
#include <QStringList>
#include <QTimer>
#include <QDebug>
class SettingsMatch : public QObject {
    Q_OBJECT
public:
    explicit SettingsMatch(QObject *parent = nullptr);
    QStringList startMatchApp(const QString &source);

private:
    void xmlElement();
    QStringList matching();

private:
    QMap<QString, QStringList> m_chine_searchList;
    QMap<QString, QStringList> m_English_searchList;
    QStringList m_chine_searchResult;
    QStringList m_English_searchResult;
    QString m_sourceText;

};

#endif // SETTINGSEARCH_H
