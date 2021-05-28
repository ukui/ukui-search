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

using namespace Zeeker;
SearchResultManager::SearchResultManager(const QString& plugin_id, QObject *parent) : QObject(parent)
{
    m_plugin_id = plugin_id;
    m_result_queue = new DataQueue<SearchPluginIface::ResultInfo>;
    m_get_result_thread = new ReceiveResultThread(m_result_queue);
    initConnections();
}

void SearchResultManager::startSearch(const QString &keyword)
{
    //NEW_TODO 加锁？停止线程？重新搜索？
//    stopSearch();
    qDebug()<<m_plugin_id<<"------------------>start by others";
    if(! m_get_result_thread->isRunning()) {
        m_get_result_thread->start();
    }
    m_result_queue->clear();
    SearchPluginIface *plugin = SearchPluginManager::getInstance()->getPlugin(m_plugin_id);
    plugin->KeywordSearch(keyword, m_result_queue);
    /*********************测试用数据*********************/
//    SearchPluginIface::ResultInfo test_info;
//    if (m_plugin_id == "File") {
//        test_info.icon = QIcon::fromTheme("ukui-control-center");
//        test_info.name = "搜索";
//        QVector<SearchPluginIface::DescriptionInfo> desc;
//        SearchPluginIface::DescriptionInfo desc_1;
//        desc_1.key = "描述";
//        desc_1.value = "控制面板搜索插件";
//        desc.append(desc_1);
//        QStringList actions;
//        actions.append("打开");
//        test_info.description = desc;
//        test_info.actionList = actions;
//        m_result_queue->append(test_info);
//    } else {
//        test_info.icon = QIcon::fromTheme("unknown");
//        test_info.name = "文件12345abcde.txt";
//        QVector<SearchPluginIface::DescriptionInfo> desc;
//        SearchPluginIface::DescriptionInfo desc_1;
//        SearchPluginIface::DescriptionInfo desc_2;
//        desc_1.key = "描述";
//        desc_1.value = "一个文件";
//        desc_2.key = "路径";
//        desc_2.value = "一个路径/a/b/c/d/e/fffffff/文件12345abcde.txt";
//        desc.append(desc_1);
//        desc.append(desc_2);
//        QStringList actions;
//        actions.append("打开");
//        actions.append("复制路径");
//        test_info.description = desc;
//        test_info.actionList = actions;
//        SearchPluginIface::ResultInfo test_info_1 = test_info;
//        test_info_1.name = "文件1";
//        SearchPluginIface::ResultInfo test_info_2 = test_info;
//        test_info_2.name = "文件2";
//        SearchPluginIface::ResultInfo test_info_3 = test_info;
//        test_info_3.name = "文件3";
//        SearchPluginIface::ResultInfo test_info_4 = test_info;
//        test_info_4.name = "文件4";
//        SearchPluginIface::ResultInfo test_info_5 = test_info;
//        test_info_5.name = "文件5";
//        SearchPluginIface::ResultInfo test_info_6 = test_info;
//        test_info_6.name = "文件6";
//        m_result_queue->append(test_info);
//        m_result_queue->append(test_info_1);
//        m_result_queue->append(test_info_2);
//        m_result_queue->append(test_info_3);
//        m_result_queue->append(test_info_4);
//        m_result_queue->append(test_info_5);
//        m_result_queue->append(test_info_6);
//    }
    /********************测试用数据********************/
}

/**
 * @brief SearchResultManager::stopSearch 停止搜索，开始一次新搜索前或主界面退出时执行
 */
void SearchResultManager::stopSearch()
{
    if(m_get_result_thread->isRunning()) {
        qDebug()<<m_plugin_id<<"-------------->stopped by others";
        m_get_result_thread->stop();
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
    this->quit();
}

//NEW_TODO 还未对队列加锁
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
            qWarning()<<"-------------->stopped by self";
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
