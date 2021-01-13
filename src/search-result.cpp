#include "search-result.h"

SearchResult::SearchResult(QObject * parent) : QThread(parent)
{
    m_mainwindow = static_cast<MainWindow *>(parent);
//    m_timer = new QTimer;
//    QObject::connect(m_timer, &QTimer::timeout, this, [ = ](){
//        qWarning()<<"-------------------------------------------------------";
//        m_timer->stop();
//        this->requestInterruption();
//    });
}

SearchResult::~SearchResult()
{
//    if (m_timer) {
//        delete m_timer;
//        m_timer = NULL;
//    }
}

void SearchResult::run()
{
    QTimer * m_timer = new QTimer;
    int emptyLists = 0;
    while(!isInterruptionRequested()) {
        qWarning()<<"--------------------";
        emptyLists = 0;
        m_mainwindow->m_searcher->m_mutex1.lock();
        if (!m_mainwindow->m_search_result_file->isEmpty()) {
            Q_EMIT this->searchResultFile(m_mainwindow->m_search_result_file->dequeue());
            m_mainwindow->m_searcher->m_mutex1.unlock();
        } else {
            emptyLists ++;
            m_mainwindow->m_searcher->m_mutex1.unlock();
        }
        m_mainwindow->m_searcher->m_mutex2.lock();
        if (!m_mainwindow->m_search_result_dir->isEmpty()) {
            Q_EMIT this->searchResultDir(m_mainwindow->m_search_result_dir->dequeue());
            m_mainwindow->m_searcher->m_mutex2.unlock();
        } else {
            emptyLists ++;
            m_mainwindow->m_searcher->m_mutex2.unlock();
        }
        m_mainwindow->m_searcher->m_mutex3.lock();
        if (!m_mainwindow->m_search_result_content->isEmpty())
            qDebug() << m_mainwindow->m_search_result_content->head();
        if (!m_mainwindow->m_search_result_content->isEmpty()) {
            Q_EMIT this->searchResultContent(m_mainwindow->m_search_result_content->dequeue());
            m_mainwindow->m_searcher->m_mutex3.unlock();
        } else {
            emptyLists ++;
            m_mainwindow->m_searcher->m_mutex3.unlock();
        }
        if (m_timer->isActive() && m_timer->remainingTime() < 0.01) {
            this->requestInterruption();
        }
        if (emptyLists == 3 && !m_timer->isActive()) {
            m_timer->start();
        } else if (emptyLists != 3) {
            m_timer->stop();
        } else {
            msleep(100);
        }
    }
}
