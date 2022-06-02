/*
 * Copyright (C) 2021, KylinSoft Co., Ltd.
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
 * Authors: zhangpengfei <zhangpengfei@kylinos.cn>
 *
 */
#include "pending-file-queue.h"
#include "file-utils.h"
#include <malloc.h>
using namespace UkuiSearch;
static PendingFileQueue *global_instance_pending_file_queue = nullptr;
PendingFileQueue::PendingFileQueue(QObject *parent) : QThread(parent), m_semaphore(INDEX_SEM, 0, QSystemSemaphore::AccessMode::Open)
{
    this->start();

    m_cacheTimer = new QTimer;
    m_minProcessTimer = new QTimer;

    m_cacheTimer->setInterval(10*1000);
    m_cacheTimer->setSingleShot(true);
    m_minProcessTimer->setInterval(500);
    m_minProcessTimer->setSingleShot(true);

    m_cacheTimer->moveToThread(this);
    m_minProcessTimer->moveToThread(this);

//    connect(this, &PendingFileQueue::cacheTimerStart, m_cacheTimer, f, Qt::DirectConnection);
//    connect(this, &PendingFileQueue::minProcessTimerStart, m_minProcessTimer, f,Qt::DirectConnection);
    connect(this, SIGNAL(cacheTimerStart()), m_cacheTimer, SLOT(start()));
    connect(this, SIGNAL(minProcessTimerStart()), m_minProcessTimer, SLOT(start()));
    connect(this, &PendingFileQueue::timerStop, m_cacheTimer, &QTimer::stop);
    connect(this, &PendingFileQueue::timerStop, m_minProcessTimer, &QTimer::stop);

    connect(m_cacheTimer, &QTimer::timeout, this, &PendingFileQueue::processCache, Qt::DirectConnection);
    connect(m_minProcessTimer, &QTimer::timeout, this, &PendingFileQueue::processCache, Qt::DirectConnection);
}

PendingFileQueue *PendingFileQueue::getInstance(QObject *parent)
{
    if (!global_instance_pending_file_queue) {
        global_instance_pending_file_queue = new PendingFileQueue(parent);
    }
    return global_instance_pending_file_queue;
}

PendingFileQueue::~PendingFileQueue()
{
    if(m_cacheTimer) {
        delete m_cacheTimer;
        m_cacheTimer = nullptr;
    }
    if(m_minProcessTimer) {
        delete m_minProcessTimer;
        m_minProcessTimer = nullptr;
    }
    global_instance_pending_file_queue = nullptr;

    IndexGenerator::getInstance()->~IndexGenerator();
}

void PendingFileQueue::forceFinish()
{
    Q_EMIT timerStop();
    this->quit();
    this->wait();
    processCache();
    m_semaphore.release(1);
}
void PendingFileQueue::enqueue(const PendingFile &file)
{
//    qDebug() << "enqueuq file: " << file.path();
    m_mutex.lock();
    m_enqueuetimes++;
    if(m_cache.isEmpty()) {
        IndexStatusRecorder::getInstance()->setStatus(INOTIFY_NORMAL_EXIT, "0");
    }
    // Remove all indexs of files under a dir which is to about be deleted,but keep delete signals.
    // Because our datebase need to delete those indexs one by one.
    if(file.shouldRemoveIndex() && file.isDir()) {
        const auto keepFile = [&file](const PendingFile& pending) {
            return (!FileUtils::isOrUnder(pending.path(), file.path()) || pending.shouldRemoveIndex());
        };
        const auto end = m_cache.end();
        const auto droppedFilesBegin = std::stable_partition(m_cache.begin(), end, keepFile);
        m_cache.erase(droppedFilesBegin, end);
    }

    if(file.shouldRemoveIndex()) {
        m_cache.removeOne(file);
    }
    int i = m_cache.indexOf(file);
    if (i == -1) {
//        qDebug() << "insert file" << file.path() << file.shouldRemoveIndex();
        m_cache << file;
    } else {
//        qDebug() << "merge file" << file.path() << file.shouldRemoveIndex();
        m_cache[i].merge(file);
    }

    if(!m_cacheTimer->isActive()) {
//        qDebug()<<"m_cacheTimer-----start!!";
//        m_cacheTimer->start();
        Q_EMIT cacheTimerStart();
    }
    Q_EMIT minProcessTimerStart();
//    m_minProcessTimer->start();
//    qDebug()<<"m_minProcessTimer-----start!!";
    m_mutex.unlock();
//    qDebug() << "Current cache-------------";
//    for(PendingFile i : m_cache) {
//        qDebug() << "|" << i.path();
//        qDebug() << "|" <<i.shouldRemoveIndex();
//    }
//    qDebug() << "Current cache-------------";
//    qDebug()<<"enqueuq file finish!!"<<file.path();
}

void PendingFileQueue::run()
{
    //阻塞线程直到first-index进程结束
    m_semaphore.acquire();
    exec();
}

void PendingFileQueue::processCache()
{
    qDebug()<< "Begin processCache!" ;
    m_mutex.lock();
    qDebug() << "Events:" << m_enqueuetimes;
    m_enqueuetimes = 0;
    m_cache.swap(m_pendingFiles);
//    m_pendingFiles = m_cache;
//    m_cache.clear();
//    m_cache.squeeze();
    m_mutex.unlock();
    qDebug() << "Current process-------------";
    for(PendingFile i : m_pendingFiles) {
        qDebug() << "|" << i.path();
        qDebug() << "|" <<i.shouldRemoveIndex();
    }
    qDebug() << "Current process-------------";
    if(m_pendingFiles.isEmpty()) {
        qDebug()<< "Empty, finish processCache!";
        return;
    }
    IndexGenerator::getInstance()->updateIndex(&m_pendingFiles);
    m_mutex.lock();
    if(m_cache.isEmpty()) {
        IndexStatusRecorder::getInstance()->setStatus(INOTIFY_NORMAL_EXIT, "2");
    }
    m_mutex.unlock();
    m_pendingFiles.clear();
    m_pendingFiles.squeeze();
    malloc_trim(0);
    qDebug()<< "Finish processCache!";
    return;
}
