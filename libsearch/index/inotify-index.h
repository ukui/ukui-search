#ifndef INOTIFYINDEX_H
#define INOTIFYINDEX_H

#include <QThread>
#include <unistd.h>
#include <sys/inotify.h>
#include "index-generator.h"
#include "traverse_bfs.h"
#include "ukui-search-qdbus.h"
#include "global-settings.h"
#include "file-utils.h"

#define BUF_LEN 1024
class InotifyIndex;
static InotifyIndex* global_instance_of_index = nullptr;
class InotifyIndex : public QThread, public Traverse_BFS
{
    Q_OBJECT
public:
    static InotifyIndex* getInstance(const QString& path){
        if (!global_instance_of_index) {
            global_instance_of_index = new InotifyIndex(path);
        }
        return global_instance_of_index;
    }
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
