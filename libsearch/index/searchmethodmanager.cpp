#include "searchmethodmanager.h"
using namespace Zeeker;
SearchMethodManager::SearchMethodManager()
{
    QStringList pathList;
    pathList.append("/media/用户保险箱");
    pathList.append("/media/邮件保险箱");
    pathList.append("/media/公共保险箱");
    pathList.append("/media/备份保险箱");
    m_iw = InotifyWatch::getInstance(pathList);
}

void SearchMethodManager::searchMethod(FileUtils::SearchMethod sm) {
    qWarning() << "searchMethod start: " << static_cast<int>(sm);
    if(FileUtils::SearchMethod::INDEXSEARCH == sm || FileUtils::SearchMethod::DIRECTSEARCH == sm) {
        FileUtils::searchMethod = sm;
    } else {
        qWarning("enum class error!!!\n");
    }
    if(FileUtils::SearchMethod::INDEXSEARCH == sm && 0 == FileUtils::_index_status) {
        qWarning() << "start first index";
        m_fi.start();
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
