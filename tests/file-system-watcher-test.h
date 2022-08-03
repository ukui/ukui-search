#ifndef FILESYSTEMWATCHERTEST_H
#define FILESYSTEMWATCHERTEST_H

#include <QObject>
#include "file-system-watcher.h"

class FileSystemWatcherTest : public QObject
{
    Q_OBJECT
public:
    explicit FileSystemWatcherTest(QObject *parent = nullptr);
    void beginSignalTest();

private:
    UkuiSearch::FileSystemWatcher *m_watcher = nullptr;

};

#endif // FILESYSTEMWATCHERTEST_H
