#include "searchmethodmanager.h"

void SearchMethodManager::searchMethod(FileUtils::SearchMethod sm)
{
    qWarning() << "searchMethod start: " << static_cast<int>(sm);
    if (FileUtils::SearchMethod::INDEXSEARCH == sm || FileUtils::SearchMethod::DIRECTSEARCH == sm) {
        FileUtils::searchMethod = sm;
    } else {
        printf("enum class error!!!\n");
        qWarning("enum class error!!!\n");
    }
    if (FileUtils::SearchMethod::INDEXSEARCH == sm && 0 == FileUtils::_index_status) {
        qWarning() << "start first index";
//        m_fi = FirstIndex("/home/zhangzihao/Desktop");
        m_fi.start();
        qWarning() << "start inotify index";
//        InotifyIndex ii("/home");
//        ii.start();
        this->m_ii = InotifyIndex::getInstance("/home");
        if (!this->m_ii->isRunning()) {
            this->m_ii->start();
        }
        qDebug()<<"Search method has been set to INDEXSEARCH";
    }
    qWarning() << "searchMethod end: " << static_cast<int>(FileUtils::searchMethod);
}
