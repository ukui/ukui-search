#include "ukui-search-task.h"
#include "ukui-search-task-private.h"
#include "search-task-plugin-manager.h"
#include <QDebug>
using namespace UkuiSearch;
UkuiSearchTaskPrivate::UkuiSearchTaskPrivate(UkuiSearchTask *parent)
    : QObject(parent),
      q(parent)
{
    m_searchCotroller = std::shared_ptr<SearchController>(new SearchController());
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

void UkuiSearchTaskPrivate::setOnlySearchFile(bool onlySearchFile)
{
    m_searchCotroller->setOnlySearchFile(onlySearchFile);
}

void UkuiSearchTaskPrivate::setOnlySearchDir(bool onlySearchDir)
{
    m_searchCotroller->setOnlySearchDir(onlySearchDir);
}

void UkuiSearchTaskPrivate::setSearchOnlineApps(bool searchOnlineApps)
{
}

size_t UkuiSearchTaskPrivate::startSearch(SearchType searchtype, QString customSearchType)
{

    m_searchId = m_searchCotroller->refreshSearchId();
    if(m_searchCotroller->getDataQueue() == nullptr) {
        qWarning() << "the date queue has not been initialized, you need run init first!";
    }

    //plugin manager do async search here
    if(SearchType::Custom != searchtype) {
        SearchTaskPluginManager::getInstance()->pluginSearch(searchtype, SearchController(m_searchCotroller));
    } else {
        SearchTaskPluginManager::getInstance()->pluginSearch(customSearchType, SearchController(m_searchCotroller));

    }

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
    return d->init();
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

void UkuiSearchTask::setOnlySearchFile(bool onlySearchFile)
{
    d->setOnlySearchFile(onlySearchFile);
}

void UkuiSearchTask::setOnlySearchDir(bool onlySearchDir)
{
    d->setOnlySearchDir(onlySearchDir);
}

void UkuiSearchTask::setSearchOnlineApps(bool searchOnlineApps)
{
    d->setSearchOnlineApps(searchOnlineApps);
}

size_t UkuiSearchTask::startSearch(SearchType searchtype, QString customSearchTYpe)
{
    return d->startSearch(searchtype, customSearchTYpe);
}

void UkuiSearchTask::stop()
{
    d->stop();
}
