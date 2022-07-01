#include "signal-transformer.h"

QMutex SignalTransformer::s_mutex;

SignalTransformer *SignalTransformer::getTransformer()
{
    static SignalTransformer *instance = new SignalTransformer();
    return instance;
}

void SignalTransformer::handleItemInsert(const AppInfoResult &item)
{
    QMutexLocker locker(&s_mutex);
    m_items2BInsert.append(item);
}

void SignalTransformer::handleItemUpdate(const AppInfoResult &item)
{
    QMutexLocker locker(&s_mutex);
    m_items2BUpdate.append(item);
}

void SignalTransformer::handleItemDelete(const QString &desktopfp)
{
    QMutexLocker locker(&s_mutex);
    m_items2BDelete.append(desktopfp);
}

void SignalTransformer::handleSignalTransform()
{
    QMutexLocker locker(&s_mutex);
    Q_EMIT this->appDBItemsAdd(m_items2BInsert);
    Q_EMIT this->appDBItemsUpdate(m_items2BUpdate);
    Q_EMIT this->appDBItemsDelete(m_items2BDelete);
    m_items2BInsert.clear();
    m_items2BUpdate.clear();
    m_items2BDelete.clear();
}

SignalTransformer::SignalTransformer(QObject *parent) : QObject(parent)
{
    connect(AppDBManager::getInstance(), &AppDBManager::appDBItemAdd, this, &SignalTransformer::handleItemInsert);
    connect(AppDBManager::getInstance(), &AppDBManager::appDBItemUpdate, this, &SignalTransformer::handleItemUpdate);
    connect(AppDBManager::getInstance(), &AppDBManager::appDBItemDelete, this, &SignalTransformer::handleItemDelete);
    connect(AppDBManager::getInstance(), &AppDBManager::finishHandleAppDB, this, &SignalTransformer::handleSignalTransform);
}

