#include "ukui-search-task.h"
#include "ukui-search-task-private.h"
#include "search-task-plugin-manager.h"
#include <QDebug>
using namespace UkuiSearch;
UkuiSearchTaskPrivate::UkuiSearchTaskPrivate(UkuiSearchTask *parent)
    : QObject(parent),
      q(parent)
{
    m_searchCotroller = std::make_shared<SearchController>();
    m_uuid = QUuid::createUuid();
}

UkuiSearchTaskPrivate::~UkuiSearchTaskPrivate()
{
    this->stop();
    SearchTaskPluginManager::getInstance()->destroyPlugins(m_uuid);
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
    m_searchCotroller->setSearchOnlineApps(searchOnlineApps);
}

void UkuiSearchTaskPrivate::initSearchPlugin(SearchType searchType, const QString& customSearchType)
{
    SearchTaskPluginIface *plugin = SearchTaskPluginManager::getInstance()->initPlugins(m_uuid, searchType, customSearchType);
    if (plugin) {
        connect(plugin, &SearchTaskPluginIface::searchFinished,this, &UkuiSearchTaskPrivate::searchFinished);
        connect(plugin, &SearchTaskPluginIface::searchError,this, &UkuiSearchTaskPrivate::searchError);
    } else {
        qWarning() << "The plugin has been initialized or the plugin failed to load.";
    }
}


bool UkuiSearchTaskPrivate::setCustomResultDataType(QString customSearchType, QStringList dataType)
{
    return m_searchCotroller->setCustomResultDataType(customSearchType, dataType);
}

bool UkuiSearchTaskPrivate::setResultDataType(SearchType searchType, ResultDataTypes dataType)
{
    return m_searchCotroller->setResultDataType(searchType, dataType);
}

size_t UkuiSearchTaskPrivate::startSearch(SearchType searchtype, const QString& customSearchType)
{
    m_searchId = m_searchCotroller->refreshSearchId();
    if(m_searchCotroller->getDataQueue() == nullptr) {
        qWarning() << "the date queue has not been initialized, you need run init first!";
    }

    m_searchCotroller->refreshDataqueue();
    //plugin manager do async search here
    if (!SearchTaskPluginManager::getInstance()->startSearch(m_uuid, m_searchCotroller, searchtype, customSearchType)) {
        Q_EMIT searchError(m_searchCotroller->getCurrentSearchId(), tr("Current task uuid error or an unregistered plugin is used!"));
    }

    return m_searchId;
}

void UkuiSearchTaskPrivate::stop()
{
    m_searchCotroller->stop();
}

void UkuiSearchTaskPrivate::clearKeyWords()
{
    m_searchCotroller->clearKeyWords();
}

void UkuiSearchTaskPrivate::clearAllConditions()
{
    m_searchCotroller->clearAllConditions();
}

void UkuiSearchTaskPrivate::clearSearchDir()
{
    m_searchCotroller->clearSearchDir();
}

void UkuiSearchTaskPrivate::clearFileLabel()
{
    m_searchCotroller->clearFileLabel();
}

void UkuiSearchTaskPrivate::setPagination(unsigned int first, unsigned int maxResults)
{
    m_searchCotroller->setPagination(first, maxResults);
}

UkuiSearchTask::UkuiSearchTask(QObject *parent) : QObject(parent), d(new UkuiSearchTaskPrivate(this))
{
    connect(d, &UkuiSearchTaskPrivate::searchFinished, this, &UkuiSearchTask::searchFinished);
    connect(d, &UkuiSearchTaskPrivate::searchError, this, &UkuiSearchTask::searchError);
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

void UkuiSearchTask::initSearchPlugin(SearchType searchType)
{
    d->initSearchPlugin(searchType);
}

bool UkuiSearchTask::setResultDataType(SearchType searchType, ResultDataTypes dataType)
{
    return d->setResultDataType(searchType, dataType);
}

bool UkuiSearchTask::setCustomResultDataType(QString customSearchType, QStringList dataType)
{
    return d->setCustomResultDataType(customSearchType, dataType);
}

size_t UkuiSearchTask::startSearch(SearchType searchtype, QString customSearchType)
{
    return d->startSearch(searchtype, customSearchType);
}

void UkuiSearchTask::stop()
{
    d->stop();
}

void UkuiSearchTask::clearAllConditions()
{
    d->clearAllConditions();
}

void UkuiSearchTask::clearKeyWords()
{
    d->clearKeyWords();
}

void UkuiSearchTask::clearSearchDir()
{
    d->clearSearchDir();
}

void UkuiSearchTask::clearFileLabel()
{
    d->clearFileLabel();
}

void UkuiSearchTask::setPagination(unsigned int first, unsigned int maxResults)
{
    d->setPagination(first, maxResults);
}
