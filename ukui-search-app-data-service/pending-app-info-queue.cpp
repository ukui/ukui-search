#include "pending-app-info-queue.h"
#include "app-db-manager.h"

using namespace UkuiSearch;

QMutex PendingAppInfoQueue::s_mutex;

PendingAppInfoQueue &PendingAppInfoQueue::getAppInfoQueue()
{
    static PendingAppInfoQueue instance;
    return instance;
}

void PendingAppInfoQueue::enqueue(const PendingAppInfo &appInfo)
{
    QMutexLocker locker(&s_mutex);
    m_handleTimes++;
    int index = m_cache.indexOf(appInfo);
    if (index == -1) {
        m_cache << appInfo;
    } else {
        //已插入项操作类型为对某字段更新
        if (m_cache[index].handleType() > PendingAppInfo::HandleType::UpdateAll) {
            if (appInfo.handleType() > PendingAppInfo::HandleType::UpdateAll) {
                m_cache[index].merge(appInfo);
            } else {
                m_cache[index].setHandleType(appInfo);
            }
        } else {
            //已插入项操作类型为对全部字段进行增删改时，设置为优先级高的操作类型
            if (m_cache[index].handleType() > appInfo.handleType()) {
                m_cache[index].setHandleType(appInfo);
            }
        }
    }

    //启动定时器
    if (!m_maxProcessTimer->isActive()) {
        qDebug() << "start max timer";
        Q_EMIT this->startMaxProcessTimer();
    }
    Q_EMIT this->startMinProcessTimer();
}

void PendingAppInfoQueue::run()
{
    exec();
}

PendingAppInfoQueue::PendingAppInfoQueue(QObject *parent) : QThread(parent)
{
    this->start();

    m_handleTimes = 0;

    m_minProcessTimer = new QTimer;
    m_maxProcessTimer = new QTimer;

    m_minProcessTimer->setInterval(500);
    m_maxProcessTimer->setInterval(5*1000);

    m_minProcessTimer->setSingleShot(true);
    m_maxProcessTimer->setSingleShot(true);

    m_minProcessTimer->moveToThread(this);
    m_maxProcessTimer->moveToThread(this);

    connect(this, SIGNAL(startMinProcessTimer()), m_minProcessTimer, SLOT(start()));
    connect(this, SIGNAL(startMaxProcessTimer()), m_maxProcessTimer, SLOT(start()));

    connect(this, &PendingAppInfoQueue::stopTimer, m_minProcessTimer, &QTimer::stop);
    connect(this, &PendingAppInfoQueue::stopTimer, m_maxProcessTimer, &QTimer::stop);

    connect(m_minProcessTimer, &QTimer::timeout, this, &PendingAppInfoQueue::processCache);
    connect(m_maxProcessTimer, &QTimer::timeout, this, &PendingAppInfoQueue::processCache);
}

PendingAppInfoQueue::~PendingAppInfoQueue()
{
    if (m_minProcessTimer) {
        delete m_minProcessTimer;
    }
    if (m_maxProcessTimer) {
        delete m_maxProcessTimer;
    }
}

void PendingAppInfoQueue::processCache()
{
    s_mutex.lock();
    qDebug() << "handle apps:" << m_handleTimes;
    m_handleTimes = 0;
    m_cache.swap(m_pendingAppInfos);
    s_mutex.unlock();

    if (m_pendingAppInfos.isEmpty()) {
        qWarning() << "No App Info will be process, I'll stop process cache of app queue.";
        return;
    }

    if (AppDBManager::getInstance()->startTransaction()) {
        for (const PendingAppInfo &info : m_pendingAppInfos) {
            PendingAppInfo::HandleTypes handleTypes = info.handleType();
            if (handleTypes < PendingAppInfo::HandleType::UpdateAll) {
                switch (handleTypes) {
                case PendingAppInfo::HandleType::Delete:
                    AppDBManager::getInstance()->handleDBItemDelete(info.path());
                    break;
                case PendingAppInfo::HandleType::Insert:
                    AppDBManager::getInstance()->handleDBItemInsert(info.path());
                    break;
                case PendingAppInfo::HandleType::UpdateAll:
                    AppDBManager::getInstance()->handleDBItemUpdate(info.path());
                    break;
                default:
                    break;
                }
            } else {
                if (handleTypes & PendingAppInfo::HandleType::UpdateLocaleData) {
                    AppDBManager::getInstance()->handleLocaleDataUpdate(info.path());
                }
                if (handleTypes & PendingAppInfo::HandleType::UpdateLaunchTimes) {
                    AppDBManager::getInstance()->handleLaunchTimesUpdate(info.path());
                }
                if (handleTypes & PendingAppInfo::HandleType::UpdateFavorites) {
                    AppDBManager::getInstance()->handleFavoritesStateUpdate(info.path(), info.favoritesState());
                }
                if (handleTypes & PendingAppInfo::HandleType::UpdateTop) {
                    AppDBManager::getInstance()->handleTopStateUpdate(info.path(), info.topState());
                }
                if (handleTypes & PendingAppInfo::HandleType::UpdateLock) {
                    AppDBManager::getInstance()->handleLockStateUpdate(info.path(), info.lockState());
                }
            }
        }

        if (AppDBManager::getInstance()->startCommit()) {
            Q_EMIT AppDBManager::getInstance()->finishHandleAppDB();
        }

        m_pendingAppInfos.clear();
        m_pendingAppInfos.squeeze();
    }
}

