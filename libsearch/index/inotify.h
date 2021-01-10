//aborted --MouseZhangZh
#ifndef INOTIFY_H
#define INOTIFY_H

#include <QObject>
#include <QThread>
#include <unistd.h>
#include <sys/inotify.h>
#include "traverse_bfs.h"
#include "messagelist-manager.h"
#define BUF_LEN 1024

class InotifyManagerRefact : public QThread, public Traverse_BFS
{
    Q_OBJECT
public:
    explicit InotifyManagerRefact(const QString&);
    ~InotifyManagerRefact();

    bool AddWatch(const QString&);
    bool RemoveWatch(const QString&);
    virtual void DoSomething(const QFileInfo &) final;
    void SendRestMessage();
Q_SIGNALS:
protected:
    void run() override;
private:
    QString *m_watch_path;
    int m_fd;
    QMap<int, QString> currentPath;
    QMap<int, QString> num2string;
//    MessageListManager* mlm;

    QMap<QString, QStringList>* dirPath;
};

#endif // INOTIFY_H
