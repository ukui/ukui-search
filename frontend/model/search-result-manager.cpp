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
#include "search-result-manager.h"

using namespace UkuiSearch;
SearchResultManager::SearchResultManager(const QString& plugin_id, QObject *parent) : QObject(parent)
{
    m_plugin_id = plugin_id;
    m_result_queue = new DataQueue<SearchPluginIface::ResultInfo>;
    m_get_result_thread = new ReceiveResultThread(m_result_queue, this);
    initConnections();
}

void SearchResultManager::startSearch(const QString &keyword)
{
    qDebug()<<m_plugin_id<<"started";
    if(! m_get_result_thread->isRunning()) {
        m_get_result_thread->start();
    }
    m_result_queue->clear();
    SearchPluginIface *plugin = SearchPluginManager::getInstance()->getPlugin(m_plugin_id);
    plugin->KeywordSearch(keyword, m_result_queue);
}

/**
 * @brief SearchResultManager::stopSearch 停止搜索，开始一次新搜索前或主界面退出时执行
 */
void SearchResultManager::stopSearch()
{
    if(m_get_result_thread->isRunning()) {
        qDebug()<<m_plugin_id<<"stopped";
        m_get_result_thread->stop();
        SearchPluginIface *plugin = SearchPluginManager::getInstance()->getPlugin(m_plugin_id);
        plugin->stopSearch();
//        m_get_result_thread->quit();
    }
}

void SearchResultManager::initConnections()
{
    connect(m_get_result_thread, &ReceiveResultThread::gotResultInfo, this, &SearchResultManager::gotResultInfo);
}

ReceiveResultThread::ReceiveResultThread(DataQueue<SearchPluginIface::ResultInfo> * result_queue, QObject *parent)
{
    m_result_queue = result_queue;
}

void ReceiveResultThread::stop()
{
    this->requestInterruption();
    this->wait();
    this->quit();
}

void ReceiveResultThread::run()
{
    QTimer * m_timer = new QTimer;
    m_timer->setInterval(3000);
    bool is_empty;
    while(!isInterruptionRequested()) {
        is_empty = false;
        if(!m_result_queue->isEmpty()) {
            Q_EMIT this->gotResultInfo(m_result_queue->dequeue());

        } else {
            is_empty = true;
        }
        if(m_timer->isActive() && m_timer->remainingTime() < 0.01) {
            this->requestInterruption();
            qWarning()<<"-------------->stopped by itself";
        }
        if(is_empty && !m_timer->isActive()) {
            m_timer->start();
        } else if(!is_empty) {
            m_timer->stop();
        } else {
            msleep(100);
        }
    }
}
