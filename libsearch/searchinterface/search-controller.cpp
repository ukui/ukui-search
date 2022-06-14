#include "search-controller.h"
#include "search-controller-private.h"
#include "ukui-search-task.h"
#include <QDebug>
using namespace UkuiSearch;
SearchControllerPrivate::SearchControllerPrivate(SearchController *parent)
    : q(parent),
      m_formerController(q->m_parent) //如果构造参数里包含父节点智能指针，则子节点带有一个智能指针指向父节点
{
    copyData();
}

SearchControllerPrivate::~SearchControllerPrivate()
{
}

size_t SearchControllerPrivate::refreshSearchId()
{
    m_searchIdMutex.lock();
    m_searchId += 1;
    m_searchIdMutex.unlock();
    return m_searchId;
}

DataQueue<ResultItem> *SearchControllerPrivate::refreshDataqueue()
{
    if(!m_sharedDataQueue.get()) {
//        m_dataQueue = new DataQueue<ResultItem>;
        m_sharedDataQueue = std::make_shared<DataQueue<ResultItem>>();
        return m_sharedDataQueue.get();
    }
    m_sharedDataQueue.get()->clear();
    return m_sharedDataQueue.get();
}

DataQueue<ResultItem> *SearchControllerPrivate::initDataQueue()
{
    if(!m_sharedDataQueue.get()) {
        m_sharedDataQueue = std::make_shared<DataQueue<ResultItem>>();
        return m_sharedDataQueue.get();
    }
    return m_sharedDataQueue.get();
}

void SearchControllerPrivate::addSearchDir(QString &path)
{
    m_searchDirs.append(path);
}

void SearchControllerPrivate::setRecurse(bool recurse)
{
    m_recurse = recurse;
}

void SearchControllerPrivate::addKeyword(QString &keyword)
{
    m_keywords.append(keyword);
}

void SearchControllerPrivate::setActiveKeywordSegmentation(bool active)
{
    m_activeKeywordSegmentation = active;
}

void SearchControllerPrivate::addFileLabel(QString &label)
{
    m_FileLabels.append(label);
}

void SearchControllerPrivate::setOnlySearchFile(bool onlySearchFile)
{
    m_onlySearchFile = onlySearchFile;
}

void SearchControllerPrivate::setOnlySearchDir(bool onlySearchDir)
{
    m_onlySearchDir = onlySearchDir;
}

void SearchControllerPrivate::setSearchOnlineApps(bool searchOnlineApps)
{
    m_searchOnlineApps = searchOnlineApps;
}

size_t SearchControllerPrivate::getCurrentSearchId()
{
    m_searchIdMutex.lock();
    size_t searchId = m_searchId;
    m_searchIdMutex.unlock();

    return searchId;
}

DataQueue<ResultItem> *SearchControllerPrivate::getDataQueue()
{
    return m_sharedDataQueue.get();
}

ResultDataTypes SearchControllerPrivate::getResultDataType(SearchType searchType)
{
    return m_searchType2ResultDataType[searchType];
}

QStringList SearchControllerPrivate::getCustomResultDataType(QString customSearchType)
{
    return m_customSearchType2ResultDataType[customSearchType];
}

bool SearchControllerPrivate::beginSearchIdCheck(size_t searchId)
{
    if(q->m_parent) {
        return q->m_parent->beginSearchIdCheck(searchId);
    }
    m_searchIdMutex.lock();
    return m_searchId == searchId;
}

void SearchControllerPrivate::finishSearchIdCheck()
{
    if(q->m_parent) {
        return q->m_parent->finishSearchIdCheck();
    }
    m_searchIdMutex.unlock();
    return;
}

void SearchControllerPrivate::stop()
{
    m_searchIdMutex.lock();
    m_searchId += 1;
    m_searchIdMutex.unlock();
}

QStringList SearchControllerPrivate::getSearchDir()
{
    return m_searchDirs;
}

bool SearchControllerPrivate::isRecurse()
{
     return m_recurse;
}

QStringList SearchControllerPrivate::getKeyword()
{
    return m_keywords;
}

bool SearchControllerPrivate::isKeywordSegmentationActived()
{
    return m_activeKeywordSegmentation;
}

QStringList SearchControllerPrivate::getFileLabel()
{
    return m_FileLabels;
}

bool SearchControllerPrivate::isSearchFileOnly()
{
    return m_onlySearchFile;
}

bool SearchControllerPrivate::isSearchDirOnly()
{
    return m_onlySearchDir;
}

bool SearchControllerPrivate::isSearchOnlineApps()
{
    return m_searchOnlineApps;
}

void SearchControllerPrivate::copyData()
{
    if(m_formerController.get()) {
        m_searchId = m_formerController.get()->getCurrentSearchId();
        //所有子节点都有一个指向根节点的队列的智能指针
        m_sharedDataQueue = m_formerController.get()->d->m_sharedDataQueue;
        m_keywords = m_formerController.get()->getKeyword();
        m_searchDirs = m_formerController.get()->getSearchDir();
        m_FileLabels = m_formerController.get()->getFileLabel();
        m_onlySearchFile = m_formerController.get()->isSearchFileOnly();
        m_onlySearchDir = m_formerController.get()->isSearchDirOnly();
        m_recurse = m_formerController.get()->isRecurse();
        m_activeKeywordSegmentation = m_formerController.get()->isKeywordSegmentationActived();
    }
}

void SearchControllerPrivate::clearAllConditions()
{
    clearKeyWords();
    clearSearchDir();
    clearFileLabel();
}

void SearchControllerPrivate::clearKeyWords()
{
    m_keywords.clear();
}

void SearchControllerPrivate::clearSearchDir()
{
    m_searchDirs.clear();
}

void SearchControllerPrivate::clearFileLabel()
{
    m_FileLabels.clear();
}

bool SearchControllerPrivate::setResultDataType(SearchType searchType, ResultDataTypes dataType)
{
    bool res(true);
    m_searchType2ResultDataType[searchType] = dataType;
    return res;
}

bool SearchControllerPrivate::setCustomResultDataType(QString customSearchType, QStringList dataType)
{
    bool res(true);
    m_customSearchType2ResultDataType[customSearchType] = dataType;
    return res;
}

void SearchControllerPrivate::setPagination(unsigned int first, unsigned int maxResults)
{
    m_first = first;
    m_maxResults = maxResults;
}

unsigned int SearchControllerPrivate::first() const
{
    return m_first;
}

unsigned int SearchControllerPrivate::maxResults() const
{
    return m_maxResults;
}

SearchController::SearchController(std::shared_ptr<SearchController> parent) : m_parent(parent), d(new SearchControllerPrivate(this))
{
}

SearchController::~SearchController()
{
    if(d) {
        delete d;
        d = nullptr;
    }
}

DataQueue<ResultItem> *SearchController::refreshDataqueue()
{
    return d->refreshDataqueue();
}

size_t SearchController::refreshSearchId()
{
    return d->refreshSearchId();
}

DataQueue<ResultItem> *SearchController::initDataQueue()
{
    return d->initDataQueue();
}

void SearchController::addSearchDir(QString &path)
{
    return d->addSearchDir(path);
}

void SearchController::setRecurse(bool recurse)
{
    d->setRecurse(recurse);
}

void SearchController::addKeyword(QString &keyword)
{
    d->addKeyword(keyword);
}

size_t SearchController::getCurrentSearchId()
{
    return d->getCurrentSearchId();
}

DataQueue<ResultItem> *SearchController::getDataQueue()
{
    return d->getDataQueue();
}

ResultDataTypes SearchController::getResultDataType(SearchType searchType)
{
    return d->getResultDataType(searchType);
}

QStringList SearchController::getCustomResultDataType(QString customSearchType)
{
    return d->getCustomResultDataType(customSearchType);
}

void SearchController::setActiveKeywordSegmentation(bool active)
{
    d->setActiveKeywordSegmentation(active);
}

void SearchController::addFileLabel(QString &label)
{
    d->addFileLabel(label);
}

void SearchController::setOnlySearchFile(bool onlySearchFile)
{
    d->setOnlySearchFile(onlySearchFile);
}

void SearchController::setOnlySearchDir(bool onlySearchDir)
{
    d->setOnlySearchDir(onlySearchDir);
}

void SearchController::setSearchOnlineApps(bool searchOnlineApps)
{
    d->setSearchOnlineApps(searchOnlineApps);
}

bool SearchController::beginSearchIdCheck(size_t searchId)
{
    return d->beginSearchIdCheck(searchId);
}

void SearchController::finishSearchIdCheck()
{
    d->finishSearchIdCheck();
}

QStringList SearchController::getSearchDir()
{
    return d->getSearchDir();
}

bool SearchController::isRecurse()
{
    return d->isRecurse();
}

QStringList SearchController::getKeyword()
{
    return d->getKeyword();
}

bool SearchController::isKeywordSegmentationActived()
{
    return d->isKeywordSegmentationActived();
}

QStringList SearchController::getFileLabel()
{
    return d->getFileLabel();
}

bool SearchController::isSearchFileOnly()
{
    return d->isSearchFileOnly();
}

bool SearchController::isSearchDirOnly()
{
    return d->isSearchDirOnly();
}

bool SearchController::isSearchOnlineApps()
{
    return d->isSearchOnlineApps();
}

void SearchController::stop()
{
    d->stop();
}

void SearchController::clearAllConditions()
{
    d->clearAllConditions();
}

void SearchController::clearKeyWords()
{
    d->clearKeyWords();
}

void SearchController::clearSearchDir()
{
    d->clearSearchDir();
}

void SearchController::clearFileLabel()
{
    d->clearFileLabel();
}

void SearchController::setPagination(unsigned int first, unsigned int maxResults)
{
    d->setPagination(first, maxResults);
}

unsigned int SearchController::first() const
{
    return d->first();
}

unsigned int SearchController::maxResults() const
{
    return d->maxResults();
}

bool SearchController::setResultDataType(SearchType searchType, ResultDataTypes dataType)
{
    return d->setResultDataType(searchType, dataType);
}

bool SearchController::setCustomResultDataType(QString customSearchType, QStringList dataType)
{
    return d->setCustomResultDataType(customSearchType, dataType);
}
