#include "file-search-task.h"
#include <QDir>
#include <QFile>
#include <QQueue>
#include <QDebug>
using namespace UkuiSearch;
FileSearchTask::FileSearchTask(QObject *parent)
{
    this->setParent(parent);
    qRegisterMetaType<size_t>("size_t");
    m_pool = new QThreadPool(this);
    m_pool->setMaxThreadCount(1);
}

const QString FileSearchTask::name()
{
    return "File";
}

const QString FileSearchTask::description()
{
    return "File search.";
}

QString FileSearchTask::getCustomSearchType()
{
    return "File";
}

void FileSearchTask::startSearch(std::shared_ptr<SearchController> searchController)
{
    FileSearchWorker *fileSearchWorker;
    fileSearchWorker = new FileSearchWorker(this, searchController);
    m_pool->start(fileSearchWorker);
}

void FileSearchTask::stop()
{

}

void FileSearchTask::sendFinishSignal(size_t searchId)
{
    Q_EMIT searchFinished(searchId);
}


FileSearchWorker::FileSearchWorker(FileSearchTask *fileSarchTask, std::shared_ptr<SearchController> searchController) : m_FileSearchTask(fileSarchTask), m_searchController(searchController)
{
}

void FileSearchWorker::run()
{
    qDebug() << "File search start";
    //TODO do search here
    QQueue<QString> bfs;
    bfs.enqueue(QDir::homePath());
    QFileInfoList list;
    QDir dir;
    if(true == m_searchController.get()->isSearchDirOnly()) {
         dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    } else {
        dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
        dir.setSorting(QDir::DirsFirst);
    }
    while(!bfs.empty()) {
        dir.setPath(bfs.dequeue());
        list = dir.entryInfoList();
        for (auto i : list) {
            if (i.isDir() && (!(i.isSymLink()))) {
                bfs.enqueue(i.absoluteFilePath());
            }
            bool matched = true;
            for(QString word : m_searchController.get()->getKeyword()) {
                if(!i.fileName().contains(word, Qt::CaseInsensitive)) {
                    matched = false;
//                    return;
                }
            }
            if(matched) {
                if((i.isDir() && true == m_searchController.get()->isSearchFileOnly()) ||
                   (i.isFile() && true == m_searchController.get()->isSearchDirOnly())) {
                    continue;
                } else {
                    ResultItem ri(m_searchController.get()->getCurrentSearchId(), i.absoluteFilePath());
                    if(m_searchController.get()->beginSearchIdCheck(m_searchController.get()->getCurrentSearchId())) {
                        m_searchController.get()->getDataQueue()->enqueue(ri);
                        m_searchController.get()->finishSearchIdCheck();
                    } else {
                        qDebug() << "Search id changed!";
                        m_searchController.get()->finishSearchIdCheck();
                        return;
                    }
                }
            }
        }
    }
    QMetaObject::invokeMethod(m_FileSearchTask, "searchFinished", Q_ARG(size_t, m_searchController.get()->getCurrentSearchId()));
}
