#ifndef INOTIFYWATCH_H
#define INOTIFYWATCH_H

#include <QThread>
#include <QBuffer>
#include <QSocketNotifier>
#include <QDataStream>
#include <QSharedMemory>

#include <sys/prctl.h>
#include <sys/wait.h>
#include <sys/inotify.h>
#include <unistd.h>

#include "traverse_bfs.h"
#include "ukui-search-qdbus.h"
#include "index-status-recorder.h"
#include "file-utils.h"
#include "pending-file-queue.h"
#include "common.h"
namespace UkuiSearch {
class InotifyWatch : public QThread, public Traverse_BFS
{
    Q_OBJECT
public:
    static InotifyWatch* getInstance();
    bool addWatch(const QString &path);
    bool removeWatch(const QString &path, bool removeFromDatabase = true);
    virtual void DoSomething(const QFileInfo &info) final;

    void firstTraverse();
    void stopWatch();
protected:
    void run() override;

private Q_SLOTS:
    void slotEvent(char *buf, ssize_t len);
private:
    explicit InotifyWatch();
    ~InotifyWatch();
    char * filter();
    void eventProcess(int socket);
    void eventProcess(const char *buffer, ssize_t len);

    int m_inotifyFd;
    QSocketNotifier* m_notifier = nullptr;
    QSharedMemory *m_sharedMemory = nullptr;
    QMap<int, QString> currentPath;
    QMutex m_mutex;


};
}
#endif // INOTIFYWATCH_H
