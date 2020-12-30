#ifndef INOTIFYMANAGER_H
#define INOTIFYMANAGER_H

#include <QObject>
#include <QThread>
#include <unistd.h>
#include <sys/inotify.h>
#include <QDebug>
#include <QDir>
#include <QQueue>
#include "messagelist-manager.h"
//#define EVENT_NUM 12
#define BUF_LEN 1024


class InotifyManager : public QThread
{
    Q_OBJECT
public:
    explicit InotifyManager();
    //the DFS method is aborted
    bool Traverse_DFS(const QString&, const bool&);//true->create, false->delete
    bool Traverse_BFS(const QString&, int autoSendMessageLength = 80000);
    //typedef bool (*AddWatch)(const QString&);
    //AddWatch cmp;

    bool AddWatch(const QString&);
    bool AddWatchList(const QStringList&);
    bool RemoveWatch(const QString&);

protected:
    void run() override;
private:
    QString *m_watch_path;
    int m_fd;
    QMap<int, QString> currentPath;
    QMap<int, QString> num2string;
    MessageListManager* mlm;

};

void testTraverse(void);

#endif // INOTIFYMANAGER_H
