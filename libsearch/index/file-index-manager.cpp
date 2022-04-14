#include "file-index-manager.h"
#include "dir-watcher.h"
#include "common.h"
using namespace UkuiSearch;
static FileIndexManager* global_instance = nullptr;
FileIndexManager::FileIndexManager(QObject *parent) : QObject(parent), m_semaphore(INDEX_SEM, 1, QSystemSemaphore::AccessMode::Create)
{
    m_fi = FirstIndex::getInstance();
    m_iw = InotifyWatch::getInstance();
}

FileIndexManager *FileIndexManager::getInstance()
{
    if(!global_instance) {
            global_instance = new FileIndexManager();
        }
       return global_instance;
}

void FileIndexManager::searchMethod(FileUtils::SearchMethod sm) {
    qWarning() << "searchMethod start: " << static_cast<int>(sm);
    if(FileUtils::SearchMethod::INDEXSEARCH == sm || FileUtils::SearchMethod::DIRECTSEARCH == sm) {
        FileUtils::searchMethod = sm;
    } else {
        qWarning("enum class error!!!\n");
    }
    if(FileUtils::SearchMethod::INDEXSEARCH == sm && 0 == FileUtils::indexStatus) {
        qDebug() << "start first index";
        m_semaphore.acquire();
        m_fi->start();
        qDebug() << "start inotify index";
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

void FileIndexManager::initIndexPathSetFunction()
{
    const QByteArray id(UKUI_SEARCH_SCHEMAS);
    if(QGSettings::isSchemaInstalled(id)) {
        m_searchSettings = new QGSettings(id);
        if(!m_searchSettings->keys().contains(SEARCH_METHOD_KEY)) {
            qWarning() << "Can not find gsettings key:" << UKUI_SEARCH_SCHEMAS << SEARCH_METHOD_KEY;
            return;
        }
    } else {
        qWarning() << "Can not find gsettings:" << UKUI_SEARCH_SCHEMAS;
        return;
    }

    connect(DirWatcher::getDirWatcher(), &DirWatcher::appendIndexItem, this, &FileIndexManager::handleIndexPathAppend, Qt::QueuedConnection);

    DirWatcher::getDirWatcher()->initDbusService();
}

void FileIndexManager::handleIndexPathAppend(const QString path, const QStringList blockList)
{
    if(!m_searchSettings->get(SEARCH_METHOD_KEY).toBool()) {
        m_searchSettings->set(SEARCH_METHOD_KEY, true);
    } else {
        m_fi->addIndexPath(path, blockList);
        m_iw->addIndexPath(path, blockList);
    }
}
