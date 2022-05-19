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
#include "search-worker-manager.h"

using namespace UkuiSearch;

SearchWorker::SearchWorker(const QString &pluginId)
{
    m_pluginId = pluginId;
    m_resultQueue = new DataQueue<SearchPluginIface::ResultInfo>;
}

SearchWorker::~SearchWorker()
{
    if(m_resultQueue) {
        delete m_resultQueue;
        m_resultQueue = nullptr;
    }
}

void SearchWorker::startSearch(QString keyWord)
{
    SearchPluginManager::getInstance()->getPlugin(m_pluginId)->KeywordSearch(keyWord, m_resultQueue);
    if(!isRunning()) {
        start();
    }
}

void SearchWorker::stop()
{
    this->requestInterruption();
    SearchPluginManager::getInstance()->getPlugin(m_pluginId)->stopSearch();
}

void SearchWorker::run()
{
    m_timer.setInterval(3000);
    bool is_empty;
    while(!isInterruptionRequested()) {
        is_empty = false;
        if(!m_resultQueue->isEmpty()) {
            Q_EMIT this->gotResultInfo(m_resultQueue->dequeue(), m_pluginId);

        } else {
            is_empty = true;
        }
        if(m_timer.isActive() && m_timer.remainingTime() < 0.01) {
            this->requestInterruption();
            qWarning()<<"Search worker for " << m_pluginId << "done.";
        }
        if(is_empty && !m_timer.isActive()) {
            m_timer.start();
        } else if(!is_empty) {
            m_timer.stop();
        } else {
            msleep(100);
        }
    }
}

SearchWorkerManager::SearchWorkerManager(QObject *parent) : QObject(parent)
{
    for(QString id : SearchPluginManager::getInstance()->getPluginIds()) {
        registerWorker(id);
    }
}

void SearchWorkerManager::registerWorker(QString pluginId)
{
    m_searchWorkers[pluginId] = new SearchWorker(pluginId);
    connect(m_searchWorkers.value(pluginId), &SearchWorker::gotResultInfo, this, &SearchWorkerManager::gotResultInfo);
    Q_EMIT newSearchWorkerEnable(pluginId);
}

bool SearchWorkerManager::unregisterWorker(const QString &pluginId)
{
    if(m_searchWorkers.contains(pluginId)) {
        delete m_searchWorkers.value(pluginId);
        m_searchWorkers.remove(pluginId);
        return true;
    } else {
        return false;
    }
}

QStringList SearchWorkerManager::getPluginIds()
{
    return m_searchWorkers.keys();
}

void SearchWorkerManager::startSearch(const QString &keyword)
{

    for(auto i : m_searchWorkers.values()) {
       i->startSearch(keyword);
    }
}

/**
 * @brief SearchResultManager::stopSearch 停止搜索，开始一次新搜索前或主界面退出时执行
 */
void SearchWorkerManager::stopSearch()
{
    for(auto i : m_searchWorkers.values()) {
       i->stop();
    }
}


