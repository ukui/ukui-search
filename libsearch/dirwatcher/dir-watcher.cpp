#include "dir-watcher.h"

#include <QDebug>
#include <QThread>

static std::once_flag flag;
static DirWatcher *global_intance = nullptr;

DirWatcher::DirWatcher(QObject *parent) : QObject(parent)
{
    m_dbusInterface = new QDBusInterface("com.ukui.search.fileindex.service",
                                         "/org/ukui/search/privateDirWatcher",
                                         "org.ukui.search.fileindex");
    if (!m_dbusInterface->isValid()) {
        qCritical() << "Create privateDirWatcher Interface Failed Because: " << QDBusConnection::sessionBus().lastError();
        return;
    }
}

DirWatcher *DirWatcher::getDirWatcher()
{
    std::call_once(flag, [ & ] {
        global_intance = new DirWatcher();
    });
    return global_intance;
}

void DirWatcher::initDbusService()
{
    QDBusReply<void> reply = m_dbusInterface->call("initDbusService");
    if (!reply.isValid()) {
        qCritical() << "initDbusService call filed!";
    }
}

QStringList DirWatcher::currentIndexableDir()
{

    QDBusReply<QStringList> reply = m_dbusInterface->call("currentIndexableDir");
    if (!reply.isValid()) {
        qCritical() << "currentIndexableDir call filed!";
        return QStringList();
    }
    return reply.value();
}

QStringList DirWatcher::currentBlackListOfIndex()
{
    QDBusReply<QStringList> reply = m_dbusInterface->call("currentBlackListOfIndex");
    if (!reply.isValid()) {
        qCritical() << "currentBlackListOfIndex call filed!";
        return QStringList();
    }
    return reply.value();
}

/**
 * @brief DirWatcher::blackListOfDir 根据传入目录返回当前目录下的所有不可搜索目录，没有则返回空列表
 * @param dirPath 要搜索的目录
 */
QStringList DirWatcher::blackListOfDir(const QString &dirPath)
{
    QDBusReply<QStringList> reply = m_dbusInterface->call("blackListOfDir", dirPath);
    if (!reply.isValid()) {
        qCritical() << "blackListOfDir call filed!";
        return QStringList();
    }
    return reply.value();
}

QStringList DirWatcher::currentSearchableDir()
{
    QDBusReply<QStringList> reply = m_dbusInterface->call("currentSearchableDir");
    if (!reply.isValid()) {
        qCritical() << "currentSearchableDir call filed!";
        return QStringList();
    }
    return reply.value();
}

QStringList DirWatcher::searchableDirForSearchApplication()
{
    QDBusReply<QStringList> reply = m_dbusInterface->call("searchableDirForSearchApplication");
    if (!reply.isValid()) {
        qCritical() << "searchableDirForSearchApplication call filed!";
        return QStringList();
    }
    return reply.value();
}

void DirWatcher::appendIndexableListItem(const QString &path)
{
    QDBusReply<void> reply = m_dbusInterface->call("appendIndexableListItem", path);
    if (!reply.isValid()) {
        qCritical() << "appendIndexableListItem call filed!";
    }
}

void DirWatcher::removeIndexableListItem(const QString &path)
{
    QDBusReply<void> reply = m_dbusInterface->call("removeIndexableListItem", path);
    if (!reply.isValid()) {
        qCritical() << "removeIndexableListItem call filed!";
    }
}
