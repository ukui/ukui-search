#include "search-method-manager.h"
using namespace UkuiSearch;
static SearchMethodManager* global_instance = nullptr;
SearchMethodManager::SearchMethodManager() : m_semaphore(INDEX_SEM, 1, QSystemSemaphore::AccessMode::Create)
{
    qDebug() << m_semaphore.errorString();
    m_fi = FirstIndex::getInstance();
    m_iw = InotifyWatch::getInstance();
}

SearchMethodManager *SearchMethodManager::getInstance()
{
    if(!global_instance) {
            global_instance = new SearchMethodManager();
        }
       return global_instance;
}

void SearchMethodManager::searchMethod(FileUtils::SearchMethod sm) {
    qWarning() << "searchMethod start: " << static_cast<int>(sm);
    if(FileUtils::SearchMethod::INDEXSEARCH == sm || FileUtils::SearchMethod::DIRECTSEARCH == sm) {
        FileUtils::searchMethod = sm;
    } else {
        qWarning("enum class error!!!\n");
    }
    if(FileUtils::SearchMethod::INDEXSEARCH == sm && 0 == FileUtils::indexStatus) {

        // Create a fifo at ~/.config/org.ukui/ukui-search, the fifo is used to control the order of child processes' running.
        QDir fifoDir = QDir(QDir::homePath() + "/.config/org.ukui/ukui-search");
        if(!fifoDir.exists())
            qDebug() << "create fifo path" << fifoDir.mkpath(fifoDir.absolutePath());

        unlink(UKUI_SEARCH_PIPE_PATH);
        int retval = mkfifo(UKUI_SEARCH_PIPE_PATH, 0777);
        if(retval == -1) {
            qCritical() << "creat fifo error!!";
            syslog(LOG_ERR, "creat fifo error!!\n");
            assert(false);
            return;
        }
        qDebug() << "create fifo success\n";
        qWarning() << "start first index";
        m_semaphore.acquire();
        m_fi->start();
        qWarning() << "start inotify index";
//        InotifyIndex ii("/home");
//        ii.start();
//        this->m_ii = InotifyIndex::getInstance("/home");
//        if(!this->m_ii->isRunning()) {
//            this->m_ii->start();
//        }
        if(!this->m_iw->isRunning()) {
            this->m_iw->start();
        }
        qDebug() << "Search method has been set to INDEXSEARCH";
    }
    if(FileUtils::SearchMethod::DIRECTSEARCH == sm) {
        m_iw->stopWatch();
    }
    qWarning() << "searchMethod end: " << static_cast<int>(FileUtils::searchMethod);
}
