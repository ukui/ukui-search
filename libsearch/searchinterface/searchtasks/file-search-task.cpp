#include "file-search-task.h"
using namespace UkuiSearch;
FileSearchTask::FileSearchTask(SearchController searchController): m_searchControl(searchController)
{
}

const QString FileSearchTask::name()
{

}

const QString FileSearchTask::description()
{

}

QString FileSearchTask::getSearchType()
{

}

void FileSearchTask::startSearch(SearchController searchController)
{

}

void FileSearchTask::stop()
{

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
