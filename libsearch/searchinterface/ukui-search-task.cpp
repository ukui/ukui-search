#include "ukui-search-task.h"
#include "ukui-search-task-private.h"
#include <QDebug>
using namespace UkuiSearch;
UkuiSearchTaskPrivate::UkuiSearchTaskPrivate(UkuiSearchTask *parent)
    : QObject(parent),
      q(parent)
{
    m_searchCotroller = new SearchController();
}

UkuiSearchTaskPrivate::~UkuiSearchTaskPrivate()
{
    this->stop();
}

DataQueue<ResultItem> *UkuiSearchTaskPrivate::init()
{
    return m_searchCotroller->initDataQueue();
}

void UkuiSearchTaskPrivate::addSearchDir(QString &path)
{
    m_searchCotroller->addSearchDir(path);
}

void UkuiSearchTaskPrivate::setRecurse(bool recurse)
{
    m_searchCotroller->setRecurse(recurse);
}

void UkuiSearchTaskPrivate::addKeyword(QString &keyword)
{
    m_searchCotroller->addKeyword(keyword);
}

void UkuiSearchTaskPrivate::addFileLabel(QString &label)
{
    m_searchCotroller->addFileLabel(label);
}

void UkuiSearchTaskPrivate::setSearchOnlineApps(bool searchOnlineApps)
{
}

size_t UkuiSearchTaskPrivate::startSearch(SearchType searchtype)
{

    m_searchId = m_searchCotroller->refreshSearchId();
//    if(!m_dataQueue) {
//        qWarning() << "Please run init first!";
//        return -1;
//    }

    //plugin manager do async search here
    // plugin->search(SearchController(m_searchController))

    return m_searchId;
}

void UkuiSearchTaskPrivate::stop()
{
    m_searchCotroller->stop();
}

UkuiSearchTask::UkuiSearchTask(QObject *parent) : QObject(parent), d(new UkuiSearchTaskPrivate(this))
{
    connect(d, &UkuiSearchTaskPrivate::searchFinished, this, &UkuiSearchTask::searchFinished);
}

UkuiSearchTask::~UkuiSearchTask()
{
}

DataQueue<ResultItem> *UkuiSearchTask::init()
{
    d->init();
}

void UkuiSearchTask::addSearchDir(QString &path)
{
    d->addSearchDir(path);
}

void UkuiSearchTask::setRecurse(bool recurse)
{
    d->setRecurse(recurse);
}

void UkuiSearchTask::addKeyword(QString &keyword)
{
    d->addKeyword(keyword);
}

void UkuiSearchTask::addFileLabel(QString &label)
{
    d->addFileLabel(label);
}

void UkuiSearchTask::setSearchOnlineApps(bool searchOnlineApps)
{
    d->setSearchOnlineApps(searchOnlineApps);
}

size_t UkuiSearchTask::startSearch(SearchType searchtype)
{
    return d->startSearch(searchtype);
}

void UkuiSearchTask::stop()
{
    d->stop();
}
