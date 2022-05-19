/*
 *
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
 * Authors: zhangjiaping <zhangjiaping@kylinos.cn>
 *
 */
#ifndef SEARCHRESULTMANAGER_H
#define SEARCHRESULTMANAGER_H

#include <QQueue>
#include <QThread>
#include <QTimer>
#include <QDebug>
#include "search-plugin-manager.h"

namespace UkuiSearch {

class SearchWorker : public QThread {
    Q_OBJECT
public:
    SearchWorker(const QString &pluginId);
    ~SearchWorker();
    void operator=(SearchWorker&) = delete;
    void startSearch(QString keyWord = QString());
    void stop();
protected:
    void run() override;

private:
    QString m_pluginId;
    QTimer m_timer;
    DataQueue<SearchPluginIface::ResultInfo> * m_resultQueue = nullptr;

Q_SIGNALS:
    void gotResultInfo(const SearchPluginIface::ResultInfo&, const QString&);

};

class SearchWorkerManager : public QObject
{
    Q_OBJECT
public:
    explicit SearchWorkerManager(QObject *parent = nullptr);
    ~SearchWorkerManager() = default;

public:
    void registerWorker(QString pluginId);
    bool unregisterWorker(const QString &pluginId);
    QStringList getPluginIds();
public Q_SLOTS:
    void startSearch(const QString &keyword);
    void stopSearch();

private:
    void initConnections();
    QMap<QString, SearchWorker*> m_searchWorkers;

Q_SIGNALS:
    void gotResultInfo(const SearchPluginIface::ResultInfo&, const QString&);
    void newSearchWorkerEnable(QString &pluginId);
};
}

#endif // SEARCHRESULTMANAGER_H
