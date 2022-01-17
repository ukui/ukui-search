#include "file-search-task.h"
using namespace UkuiSearch;
FileSearchTask::FileSearchTask(SearchController searchController): m_searchControl(searchController)
{
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

void FileSearchTask::startSearch(SearchController searchController)
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

void FileSearchTask::run()
{
    //file search, based on index or direct search?
    //1.do search
    if(m_searchControl.beginSearchIdCheck(m_searchControl.getCurrentSearchId())) {

        //do enqueue here
        m_searchControl.finishSearchIdCheck();
    }
    m_searchControl.finishSearchIdCheck();
    //finish
}

FileSearchWorker::FileSearchWorker(FileSearchTask *fileSarchTask, SearchController searchController) : m_FileSearchTask(fileSarchTask), m_searchController(searchController)
{
}

void FileSearchWorker::run()
{
    //TODO do search here

    size_t searchId = m_searchController.getCurrentSearchId();
    ResultItem ri(searchId);
    if(m_searchController.beginSearchIdCheck(m_searchController.getCurrentSearchId())) {
        m_searchController.getDataQueue()->enqueue(ri);
        m_searchController.finishSearchIdCheck();
    } else {
        m_searchController.finishSearchIdCheck();
        return;
    }
    QMetaObject::invokeMethod(m_FileSearchTask, "searchFinished", Q_ARG(size_t, searchId));
}
