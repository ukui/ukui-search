#include "file-system-watcher-test.h"
#include <QDebug>
using namespace UkuiSearch;
FileSystemWatcherTest::FileSystemWatcherTest(QObject *parent) : QObject(parent)
{
    m_watcher = new FileSystemWatcher(FileSystemWatcher::WatchEvents(FileSystemWatcher::EventMove | FileSystemWatcher::EventMoveSelf |
                                                              FileSystemWatcher::EventCreate | FileSystemWatcher::EventDelete |
                                                              FileSystemWatcher::EventDeleteSelf | FileSystemWatcher::EventUnmount |
                                                              FileSystemWatcher::EventModify | FileSystemWatcher::EventAttributeChange));
}

void FileSystemWatcherTest::beginSignalTest()
{
    m_watcher->addWatch("/home/zpf/图片");

    connect(m_watcher, &FileSystemWatcher::attributeChanged,
                     [](const QString& fileUrl) { qDebug() << "AttrbuteChanged:" << fileUrl; });
    connect(m_watcher, &FileSystemWatcher::created,
                     [](const QString& fileUrl, bool isDir) { qDebug() << "Created:" << fileUrl << isDir; });
    connect(m_watcher, &FileSystemWatcher::deleted,
                     [](const QString& fileUrl, bool isDir) { qDebug() << "Deleted:" << fileUrl << isDir; });
    connect(m_watcher, &FileSystemWatcher::modified,
                     [](const QString& fileUrl) { qDebug() << "Modified:" << fileUrl; });
    connect(m_watcher, &FileSystemWatcher::moved,
                     [](const QString& fileUrl, bool isDir) { qDebug() << "Modified:" << fileUrl << isDir; });
    connect(m_watcher, &FileSystemWatcher::closedWrite,
                     [](const QString& fileUrl) { qDebug() << "ClosedWrite:"  << fileUrl; });
}
