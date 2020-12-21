#ifndef INOTIFYMANAGER_H
#define INOTIFYMANAGER_H

#include <QObject>
#include <QThread>
#include <unistd.h>
#include <sys/inotify.h>
#include <QDebug>
#include <QDir>
#include <QQueue>
//#define EVENT_NUM 12
#define BUF_LEN 1024


class InotifyManager : public QThread
{
    Q_OBJECT
public:
    explicit InotifyManager();
    bool Traverse(const QString&, const bool&);//true->create, false->delete
    bool Traverse_BFS(const QString&, const bool&);
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
};

void testTraverse(void);

#endif // INOTIFYMANAGER_H
