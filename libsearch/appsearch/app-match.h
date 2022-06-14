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
#include "../appdata/app-info-table.h"

namespace UkuiSearch {
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
    void appNameMatch(QString keyWord, size_t uniqueSymbol, DataQueue<SearchPluginIface::ResultInfo> *searchResult);
    void parseSoftWareCenterReturn(QList<QMap<QString, QString>> list, size_t uniqueSymbol, DataQueue<SearchPluginIface::ResultInfo> *searchResult);
    //void creatResultInfo(SearchPluginIface::ResultInfo &ri, QMapIterator<UkuiSearch::NameString, QStringList> &iter, bool isInstalled = true);

    AppInfoTable *m_appInfoTable = nullptr;
    QString m_sourceText;
    size_t m_uniqueSymbol;
    DataQueue<SearchPluginIface::ResultInfo> *m_search_result = nullptr;
    QDBusInterface *m_interFace = nullptr;

private Q_SLOTS:
    void slotDBusCallFinished(QString keyWord, size_t uniqueSymbol, DataQueue<SearchPluginIface::ResultInfo> *searchResult);
};
}

#endif // APPMATCH_H
