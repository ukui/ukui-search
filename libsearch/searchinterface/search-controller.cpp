#include "search-controller.h"
#include "search-controller-private.h"
#include "ukui-search-task.h"
#include <QDebug>
using namespace UkuiSearch;
SearchControllerPrivate::SearchControllerPrivate(SearchController *parent)
    : q(parent)
{
    if(q->m_parent) {
        m_formerController = QSharedPointer<SearchController>(q->m_parent);
        m_searchId = q->m_parent->d->m_searchId;
        m_dataQueue = q->m_parent->d->m_dataQueue;
        m_keywords = q->m_parent->d->m_keywords;
        m_searchDirs = q->m_parent->d->m_searchDirs;
        m_FileLabels = q->m_parent->d->m_FileLabels;
        m_recurse = q->m_parent->d->m_recurse;
        m_activeKeywordSegmentation = q->m_parent->d->m_activeKeywordSegmentation;
    }
}

SearchControllerPrivate::~SearchControllerPrivate()
{
    m_formerController.clear();
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
        return m_dataQueue;
    }
    m_dataQueue->clear();
    return m_dataQueue;;
}

DataQueue<ResultItem> *SearchControllerPrivate::initDataQueue()
{
    if(!m_dataQueue) {
        m_dataQueue = new DataQueue<ResultItem>;
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

size_t SearchControllerPrivate::getCurrentSearchId()
{
    return m_searchId;
}

DataQueue<ResultItem> *SearchControllerPrivate::getDataQueue()
{
    return m_dataQueue;
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

}

bool SearchControllerPrivate::isRecurse()
{

}

QStringList SearchControllerPrivate::getKeyword()
{

}

bool SearchControllerPrivate::isKeywordSegmentationActived()
{

}

QStringList SearchControllerPrivate::getFileLabel()
{

}

SearchController::SearchController(SearchController *parent) : m_parent(parent), d(new SearchControllerPrivate(this))
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
    d->getCurrentSearchId();
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

bool SearchController::beginSearchIdCheck(size_t searchId)
{
    return d->beginSearchIdCheck(searchId);
}

void SearchController::finishSearchIdCheck()
{
   d->finishSearchIdCheck();
}

void SearchController::stop()
{
    d->stop();
}
