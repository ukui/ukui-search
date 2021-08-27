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
#include "search-result.h"

using namespace Zeeker;
SearchResult::SearchResult(QObject * parent) : QThread(parent) {
    m_mainwindow = static_cast<MainWindow *>(parent);
//    m_timer = new QTimer;
//    QObject::connect(m_timer, &QTimer::timeout, this, [ = ](){
//        qWarning()<<"-------------------------------------------------------";
//        m_timer->stop();
//        this->requestInterruption();
//    });
}

SearchResult::~SearchResult() {
//    if (m_timer) {
//        delete m_timer;
//        m_timer = NULL;
//    }
}

void SearchResult::run() {
    QTimer * m_timer = new QTimer;
    m_timer->setInterval(30000);
    int emptyLists = 0;
    while(!isInterruptionRequested()) {
        emptyLists = 0;
        m_mainwindow->m_searcher->m_mutexFile.lock();
        if(!m_mainwindow->m_search_result_file->isEmpty()) {
            Q_EMIT this->searchResultFile(m_mainwindow->m_search_result_file->dequeue());
            m_mainwindow->m_searcher->m_mutexFile.unlock();
        } else {
            emptyLists ++;
            m_mainwindow->m_searcher->m_mutexFile.unlock();
        }
        m_mainwindow->m_searcher->m_mutexDir.lock();
        if(!m_mainwindow->m_search_result_dir->isEmpty()) {
            Q_EMIT this->searchResultDir(m_mainwindow->m_search_result_dir->dequeue());
            m_mainwindow->m_searcher->m_mutexDir.unlock();
        } else {
            emptyLists ++;
            m_mainwindow->m_searcher->m_mutexDir.unlock();
        }
        m_mainwindow->m_searcher->m_mutexContent.lock();
//        if (!m_mainwindow->m_search_result_content->isEmpty())
//            qDebug() << m_mainwindow->m_search_result_content->head();
        if(!m_mainwindow->m_search_result_content->isEmpty()) {
            Q_EMIT this->searchResultContent(m_mainwindow->m_search_result_content->dequeue());
            m_mainwindow->m_searcher->m_mutexContent.unlock();
        } else {
            emptyLists ++;
            m_mainwindow->m_searcher->m_mutexContent.unlock();
        }
        if(m_timer->isActive() && m_timer->remainingTime() < 0.01) {
            this->requestInterruption();
        }
        if(emptyLists == 3 && !m_timer->isActive()) {
            m_timer->start();
        } else if(emptyLists != 3) {
            m_timer->stop();
        } else {
            msleep(100);
        }
    }
}
