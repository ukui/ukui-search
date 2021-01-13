#include "inotify-index.h"


InotifyIndex::InotifyIndex(const QString& path) : Traverse_BFS(path)
{
    /*-------------ukuisearchdbus Test start-----------------*/
    qDebug() << "setInotifyMaxUserWatches start";
    UkuiSearchQDBus usQDBus;
    usQDBus.setInotifyMaxUserWatches();
    qDebug() << "setInotifyMaxUserWatches end";

    /*-------------ukuisearchdbus Test End-----------------*/



    m_fd = inotify_init();
    qDebug() << "m_fd----------->" <<m_fd;

    this->AddWatch("/home");
    this->Traverse();


    GlobalSettings::getInstance()->setValue(INOTIFY_NORMAL_EXIT, "0");
}

InotifyIndex::~InotifyIndex()
{
    GlobalSettings::getInstance()->setValue(INOTIFY_NORMAL_EXIT, "2");
    IndexGenerator::getInstance()->~IndexGenerator();
}

void InotifyIndex::DoSomething(const QFileInfo& fileInfo){
    if(fileInfo.isDir()){
        this->AddWatch(fileInfo.absoluteFilePath());
    }
}

bool InotifyIndex::AddWatch(const QString &path){
    int ret = inotify_add_watch(m_fd, path.toStdString().c_str(), (IN_MOVED_FROM | IN_MOVED_TO | IN_CREATE | IN_DELETE | IN_MODIFY));
    if (ret == -1) {
        qDebug() << "AddWatch error:" << path;
        return false;
    }
//    Q_ASSERT(ret != -1);
    assert(ret != -1);
    currentPath[ret] = path;
    //qDebug() << "Watch:" << path;
    return true;
}

bool InotifyIndex::RemoveWatch(const QString &path){
    int ret = inotify_rm_watch(m_fd, currentPath.key(path));
    if (ret){
        qDebug() << "remove path error";
        return false;
    }
//    Q_ASSERT(ret == 0);
    assert(ret == 0);

    for (QMap<int, QString>::Iterator i = currentPath.begin(); i != currentPath.end();){
        if (i.value().length() > path.length()){
            if (i.value().mid(0, path.length()) == path){
                qDebug() << "remove path: " << i.value();
                ret = inotify_rm_watch(m_fd, currentPath.key(path));
                if (ret){
                    qDebug() << "remove path error";
//                    return false;
                }
//                Q_ASSERT(ret == 0);
//                assert(ret == 0);
                /*--------------------------------*/
                //在此调用删除索引
                IndexGenerator::getInstance()->deleteAllIndex(new QStringList(i.value()));
                /*--------------------------------*/
                currentPath.erase(i++);
            }
            else{
                i++;
            }
        }
        else{
            i++;
        }
    }
//    qDebug() << path;
    //这个貌似不用删，先mark一下
    //currentPath.remove(currentPath.key(path));
    return true;
}

/*
 * Symbolic Link!!!!!!!!!!!!!!!!!!
 * Sysmbolic link to database dir will make a Infinite loop !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * MouseZhangZh
*/

void InotifyIndex::run(){

    char * p;
    char buf[BUF_LEN] __attribute__((aligned(8)));

    ssize_t numRead;

    QQueue<QVector<QString>>* indexQueue = new QQueue<QVector<QString>>();
    QQueue<QString>* contentIndexQueue = new QQueue<QString>();

    for (;;) { /* Read events forever */
        numRead = read(m_fd, buf, BUF_LEN);
        if (numRead == 0) {
            qDebug() << "read() from inotify fd returned 0!";
        }
        if (numRead == -1) {
            qDebug() << "read";
        }
//        qDebug() << "Read " << numRead << " bytes from inotify fd";

        /* Process all of the events in buffer returned by read() */

        for (p = buf; p < buf + numRead;) {
            struct inotify_event * event = reinterpret_cast<inotify_event *>(p);
//            qDebug() << "Read Event: " << currentPath[event->wd] << QString(event->name) << event->cookie << event->wd << event->mask;
            if(event->name[0] != '.'){
                qDebug() << QString(currentPath[event->wd] + '/' + event->name);

//                switch (event->mask) {
                if (event->mask & IN_CREATE){
                    if (event->mask & IN_ISDIR){
                        AddWatch(currentPath[event->wd] + '/' + event->name);
                        this->setPath(currentPath[event->wd] + '/' + event->name);
                        Traverse();
                    }

                    /*--------------------------------*/
//                    IndexGenerator::getInstance()->creatAllIndex(QQueue<QVector<QString>>(QVector<QString>() << fileInfo.fileName() << fileInfo.absoluteFilePath() << QString(fileInfo.isDir() ? "1" : "0")));
                    indexQueue->enqueue(QVector<QString>() << QString(event->name) << QString(currentPath[event->wd] + '/' + event->name) << QString((event->mask & IN_ISDIR) ? "1" : "0"));
                    IndexGenerator::getInstance()->creatAllIndex(indexQueue);
                    indexQueue->clear();
                    for (auto i : this->targetFileTypeVec){
                        if (QString(currentPath[event->wd] + '/' + event->name).endsWith(i)){
                            contentIndexQueue->enqueue(QString(currentPath[event->wd] + '/' + event->name));
                            IndexGenerator::getInstance()->creatAllIndex(contentIndexQueue);
                            contentIndexQueue->clear();
                            break;
                        }
                    }
                    goto next;
                }


                if ((event->mask & IN_DELETE) | (event->mask & IN_MOVED_FROM)){
                    if (event->mask & IN_ISDIR){
                        RemoveWatch(currentPath[event->wd] + '/' + event->name);
                    }
                    //delete once more
                    IndexGenerator::getInstance()->deleteAllIndex(new QStringList(currentPath[event->wd] + '/' + event->name));
                    goto next;
                }


                if (event->mask & IN_MODIFY){
                    if (!(event->mask & IN_ISDIR)){
                        IndexGenerator::getInstance()->deleteAllIndex(new QStringList(currentPath[event->wd] + '/' + event->name));
                        indexQueue->enqueue(QVector<QString>() << QString(event->name) << QString(currentPath[event->wd] + '/' + event->name) << QString((event->mask & IN_ISDIR) ? "1" : "0"));
                        IndexGenerator::getInstance()->creatAllIndex(indexQueue);
                        indexQueue->clear();
                        for (auto i : this->targetFileTypeVec){
                            if (QString(currentPath[event->wd] + '/' + event->name).endsWith(i)){
                                contentIndexQueue->enqueue(QString(currentPath[event->wd] + '/' + event->name));
                                IndexGenerator::getInstance()->creatAllIndex(contentIndexQueue);
                                contentIndexQueue->clear();
                                break;
                            }
                        }
                    }
                    goto next;
                }


                if (event->mask & IN_MOVED_TO){
                    if (event->mask & IN_ISDIR){
                        RemoveWatch(currentPath[event->wd] + '/' + event->name);
                        IndexGenerator::getInstance()->deleteAllIndex(new QStringList(currentPath[event->wd] + '/' + event->name));
                        AddWatch(currentPath[event->wd] + '/' + event->name);
                        this->setPath(currentPath[event->wd] + '/' + event->name);
                        Traverse();

                        //



                        indexQueue->enqueue(QVector<QString>() << QString(event->name) << QString(currentPath[event->wd] + '/' + event->name) << QString((event->mask & IN_ISDIR) ? "1" : "0"));
                        IndexGenerator::getInstance()->creatAllIndex(indexQueue);
                        indexQueue->clear();
                        for (auto i : this->targetFileTypeVec){
                            if (QString(currentPath[event->wd] + '/' + event->name).endsWith(i)){
                                contentIndexQueue->enqueue(QString(currentPath[event->wd] + '/' + event->name));
                                IndexGenerator::getInstance()->creatAllIndex(contentIndexQueue);
                                contentIndexQueue->clear();
                                break;
                            }
                        }
                    }
                    else {
                        IndexGenerator::getInstance()->deleteAllIndex(new QStringList(currentPath[event->wd] + '/' + event->name));
                        indexQueue->enqueue(QVector<QString>() << QString(event->name) << QString(currentPath[event->wd] + '/' + event->name) << QString((event->mask & IN_ISDIR) ? "1" : "0"));
                        IndexGenerator::getInstance()->creatAllIndex(indexQueue);
                        indexQueue->clear();
                        for (auto i : this->targetFileTypeVec){
                            if (QString(currentPath[event->wd] + '/' + event->name).endsWith(i)){
                                contentIndexQueue->enqueue(QString(currentPath[event->wd] + '/' + event->name));
                                IndexGenerator::getInstance()->creatAllIndex(contentIndexQueue);
                                contentIndexQueue->clear();
                                break;
                            }
                        }
                    }
                    goto next;
                }

//                }

//                //传创建或移动过来的文件路径
//                if((event->mask & IN_CREATE)){
//                    //添加监视要先序遍历，先添加top节点
//                    if (event->mask & IN_ISDIR){
//                        AddWatch(currentPath[event->wd] + '/' + event->name);
//                        this->setPath(currentPath[event->wd] + '/' + event->name);
//                        Traverse();
//                    }

//                    /*--------------------------------*/
////                    IndexGenerator::getInstance()->creatAllIndex(QQueue<QVector<QString>>(QVector<QString>() << fileInfo.fileName() << fileInfo.absoluteFilePath() << QString(fileInfo.isDir() ? "1" : "0")));
//                    indexQueue->enqueue(QVector<QString>() << QString(event->name) << QString(currentPath[event->wd] + '/' + event->name) << QString((event->mask & IN_ISDIR) ? "1" : "0"));
//                    IndexGenerator::getInstance()->creatAllIndex(indexQueue);
//                    indexQueue->clear();
//                    for (auto i : this->targetFileTypeVec){
//                        if (QString(currentPath[event->wd] + '/' + event->name).endsWith(i)){
//                            contentIndexQueue->enqueue(QString(currentPath[event->wd] + '/' + event->name));
//                            IndexGenerator::getInstance()->creatAllIndex(contentIndexQueue);
//                            contentIndexQueue->clear();
//                            break;
//                        }
//                    }
//                    /*--------------------------------*/
//                }
//                else if((event->mask & IN_DELETE) | (event->mask & IN_MOVED_FROM)){
//                    if (event->mask & IN_ISDIR){
//                        RemoveWatch(currentPath[event->wd] + '/' + event->name);
//                    }
//                        IndexGenerator::getInstance()->deleteAllIndex(new QStringList(currentPath[event->wd] + '/' + event->name));
//                }
//                else if((event->mask & IN_MODIFY) | (event->mask & IN_MOVED_TO)){
//                    if (!(event->mask & IN_ISDIR)){
//                        IndexGenerator::getInstance()->deleteAllIndex(new QStringList(currentPath[event->wd] + '/' + event->name));
//                        indexQueue->enqueue(QVector<QString>() << QString(event->name) << QString(currentPath[event->wd] + '/' + event->name) << QString((event->mask & IN_ISDIR) ? "1" : "0"));
//                        IndexGenerator::getInstance()->creatAllIndex(indexQueue);
//                        indexQueue->clear();
//                        for (auto i : this->targetFileTypeVec){
//                            if (QString(currentPath[event->wd] + '/' + event->name).endsWith(i)){
//                                contentIndexQueue->enqueue(QString(currentPath[event->wd] + '/' + event->name));
//                                IndexGenerator::getInstance()->creatAllIndex(contentIndexQueue);
//                                contentIndexQueue->clear();
//                                break;
//                            }
//                        }
//                    }
//                }
                /*--------------------------------*/
            }
next:
            p += sizeof(struct inotify_event) + event->len;
        }
    }

    delete indexQueue;
    indexQueue = nullptr;
    delete contentIndexQueue;
    contentIndexQueue = nullptr;
}
