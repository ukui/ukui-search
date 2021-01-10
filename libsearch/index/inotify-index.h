#ifndef INOTIFYINDEX_H
#define INOTIFYINDEX_H

#include <QThread>
#include <unistd.h>
#include <sys/inotify.h>
#include "index-generator.h"
#include "traverse_bfs.h"
#include "ukui-search-qdbus.h"

#define BUF_LEN 1024

class InotifyIndex : public QThread, public Traverse_BFS
{
    Q_OBJECT
public:
    InotifyIndex(const QString&);
    ~InotifyIndex();

    bool AddWatch(const QString&);
    bool RemoveWatch(const QString&);
    virtual void DoSomething(const QFileInfo &) final;
protected:
    void run() override;
private:
    QString* m_watch_path;
    int m_fd;

    QMap<int, QString> currentPath;
    const QVector<QString> targetFileTypeVec ={
//        QString(".doc"),
        QString(".docx"),
//        QString(".ppt"),
//        QString(".pptx"),
//        QString(".xls"),
//        QString(".xlsx"),
        QString(".txt")};
};

#endif // INOTIFYINDEX_H
