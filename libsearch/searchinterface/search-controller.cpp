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
    if(!m_dataQueue) {
        m_dataQueue = new DataQueue<ResultItem>;
        m_sharedDataueue = std::shared_ptr<DataQueue<ResultItem>>(m_dataQueue);
        return m_dataQueue;
    }
    m_dataQueue->clear();
    return m_dataQueue;;
}

DataQueue<ResultItem> *SearchControllerPrivate::initDataQueue()
{
    if(!m_dataQueue) {
        m_dataQueue = new DataQueue<ResultItem>;
        m_sharedDataueue = std::shared_ptr<DataQueue<ResultItem>>(m_dataQueue);
        return m_dataQueue;
    }
    return m_dataQueue;
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

size_t SearchControllerPrivate::getCurrentSearchId()
{
    return m_searchId;
}

DataQueue<ResultItem> *SearchControllerPrivate::getDataQueue()
{
    return m_sharedDataueue.get();
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
    m_searchId = 0;
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

void SearchControllerPrivate::copyData()
{

    if(m_formerController.get()) {
        m_searchId = m_formerController.get()->getCurrentSearchId();
        //所有子节点都有一个指向根节点的队列的智能指针
        m_sharedDataueue = m_formerController.get()->d->m_sharedDataueue;
        m_keywords = m_formerController.get()->getKeyword();
        m_searchDirs = m_formerController.get()->getSearchDir();
        m_FileLabels = m_formerController.get()->getFileLabel();
        m_onlySearchFile = m_formerController.get()->isSearchFileOnly();
        m_onlySearchDir = m_formerController.get()->isSearchDirOnly();
        m_recurse = m_formerController.get()->isRecurse();
        m_activeKeywordSegmentation = m_formerController.get()->isKeywordSegmentationActived();
    }
}

SearchController::SearchController(std::shared_ptr<SearchController> parent) : m_parent(parent), d(new SearchControllerPrivate(this))
{
}

SearchController::~SearchController()
{
    delete d;
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

void SearchController::stop()
{
    d->stop();
}
