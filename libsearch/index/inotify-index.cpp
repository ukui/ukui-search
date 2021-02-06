/*
 * Copyright (C) 2020, KylinSoft Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: zhangzihao <zhangzihao@kylinos.cn>
 *
 */
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

    this->AddWatch(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    this->setPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
//    this->Traverse();
    this->firstTraverse();


    GlobalSettings::getInstance()->setValue(INOTIFY_NORMAL_EXIT, "0");
}

InotifyIndex::~InotifyIndex()
{
    GlobalSettings::getInstance()->setValue(INOTIFY_NORMAL_EXIT, "2");
    IndexGenerator::getInstance()->~IndexGenerator();
}

void InotifyIndex::firstTraverse(){
    QQueue<QString> bfs;
    bfs.enqueue(this->path);
    QFileInfoList list;
    QDir dir;
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::DirsFirst);
    while (!bfs.empty()) {
        dir.setPath(bfs.dequeue());
        list = dir.entryInfoList();
        for (auto i : list){
            if (i.isDir()){
                this->AddWatch(i.absoluteFilePath());
                bfs.enqueue(i.absoluteFilePath());
            }
        }
    }
}

void InotifyIndex::DoSomething(const QFileInfo& fileInfo){
    qDebug() << fileInfo.fileName() << "-------" << fileInfo.absoluteFilePath();
    if(fileInfo.isDir()){
        this->AddWatch(fileInfo.absoluteFilePath());
    }
    QQueue<QVector<QString> >* tempFile = new QQueue<QVector<QString> >;
    tempFile->enqueue(QVector<QString>() << fileInfo.fileName() << fileInfo.absoluteFilePath() << QString(fileInfo.isDir() ? "1" : "0"));
    IndexGenerator::getInstance()->creatAllIndex(tempFile);
    if (tempFile)
        delete tempFile;
    for (auto i : this->targetFileTypeVec){
        if (fileInfo.fileName().endsWith(i)){
            QQueue<QString>* tempContent = new QQueue<QString>;
            tempContent->enqueue(fileInfo.absoluteFilePath());
            IndexGenerator::getInstance()->creatAllIndex(tempContent);
            if (tempContent)
                delete tempContent;
        }
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
//    qDebug() << "Watch: " << path << "ret: " << ret;
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
//        qDebug() << i.value();
        if (i.value().length() > path.length()){
//            if (i.value().mid(0, path.length()) == path){
//            if (path.startsWith(i.value())){
            if (i.value().startsWith(path)){
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
                qDebug() << i.value();
                IndexGenerator::getInstance()->deleteAllIndex(new QStringList(i.value()));
                /*--------------------------------*/
                currentPath.erase(i++);
//                i++;
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
//    currentPath.remove(currentPath.key(path));
    return true;
}

void InotifyIndex::eventProcess(const char* buf, ssize_t tmp){
    QQueue<QVector<QString>>* indexQueue = new QQueue<QVector<QString>>();
    QQueue<QString>* contentIndexQueue = new QQueue<QString>();

    ssize_t numRead = 0;
    numRead = tmp;
    char * p = const_cast<char*>(buf);

    for (; p < buf + numRead;) {
        struct inotify_event * event = reinterpret_cast<inotify_event *>(p);
        qDebug() << "Read Event event->wd: " << event->wd;
        qDebug() << "Read Event: " << currentPath[event->wd] << QString(event->name) << event->cookie << event->wd << event->mask;
        if(event->name[0] != '.'){
            qDebug() << QString(currentPath[event->wd] + '/' + event->name);
            //                switch (event->mask) {
            if (event->mask & IN_CREATE){

                //Create top dir first, traverse it last.
                qDebug() << "IN_CREATE";
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

                if (event->mask & IN_ISDIR){
                    AddWatch(currentPath[event->wd] + '/' + event->name);
                    setPath(currentPath[event->wd] + '/' + event->name);
                    Traverse();
                }
                goto next;
            }


            if ((event->mask & IN_DELETE) | (event->mask & IN_MOVED_FROM)){
                qDebug() << "IN_DELETE or IN_MOVED_FROM";
                if (event->mask & IN_ISDIR){
                    RemoveWatch(currentPath[event->wd] + '/' + event->name);
                }
                //delete once more
                IndexGenerator::getInstance()->deleteAllIndex(new QStringList(currentPath[event->wd] + '/' + event->name));
                goto next;
            }


            if (event->mask & IN_MODIFY){
                qDebug() << "IN_MODIFY";
                if (!(event->mask & IN_ISDIR)){
//                    IndexGenerator::getInstance()->deleteAllIndex(new QStringList(currentPath[event->wd] + '/' + event->name));
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
                qDebug() << "IN_MOVED_TO";
                if (event->mask & IN_ISDIR){
                    RemoveWatch(currentPath[event->wd] + '/' + event->name);
//                    IndexGenerator::getInstance()->deleteAllIndex(new QStringList(currentPath[event->wd] + '/' + event->name));


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

                    AddWatch(currentPath[event->wd] + '/' + event->name);
                    setPath(currentPath[event->wd] + '/' + event->name);
                    Traverse();

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

    delete indexQueue;
    indexQueue = nullptr;
    delete contentIndexQueue;
    contentIndexQueue = nullptr;
}

/*
 * Symbolic Link!!!!!!!!!!!!!!!!!!
 * Sysmbolic link to database dir will make a Infinite loop !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * MouseZhangZh
*/

void InotifyIndex::run(){
    int fifo_fd;
    char buffer[2];
    memset(buffer, 0, sizeof(buffer));
    fifo_fd = open(UKUI_SEARCH_PIPE_PATH, O_RDWR);
    if(fifo_fd == -1)
    {
        perror("open fifo error\n");
        assert(false);
    }
    int retval = read(fifo_fd, buffer, sizeof(buffer));
    if(retval == -1)
    {
        perror("read error\n");
        assert(false);
    }
    printf("read fifo=[%s]\n", buffer);

    printf("read data ok\n");
    close(fifo_fd);
    if (buffer[0] & 0x1){
        printf("data confirmed\n");
    }
    unlink(UKUI_SEARCH_PIPE_PATH);

    char buf[BUF_LEN] __attribute__((aligned(8)));

    ssize_t numRead;

    for (;;) { /* Read events forever */
read:
        numRead = read(m_fd, buf, BUF_LEN);

        if (numRead == -1){
            printf("\033[1;31;40mread event error\033[0m\n");
            fflush(stdout);
            assert(false);
        }


        char * tmp = const_cast<char*>(buf);

        for (; tmp < buf + numRead;) {
            struct inotify_event * event = reinterpret_cast<inotify_event *>(tmp);
    //        qDebug() << "Read Event: " << currentPath[event->wd] << QString(event->name) << event->cookie << event->wd << event->mask;
            if(event->name[0] != '.'){
                goto fork;
            }
            tmp += sizeof(struct inotify_event) + event->len;

        }
        goto read;

fork:
        ++FileUtils::_index_status;

        pid_t pid;
        pid = fork();
        if(pid  == 0)
        {
            prctl(PR_SET_PDEATHSIG, SIGTERM);
            prctl(PR_SET_NAME,"inotify-index");
            if (numRead == 0) {
                qDebug() << "read() from inotify fd returned 0!";
            }
            if (numRead == -1) {
                qDebug() << "read";
            }
            eventProcess(buf, numRead);


            fd_set read_fds;
            int rc;
            timeval* read_timeout = (timeval*)malloc(sizeof(timeval));

            read_timeout->tv_sec = 60;
            read_timeout->tv_usec = 0;
            for(;;)
            {
                FD_ZERO(&read_fds);
                FD_SET(m_fd, &read_fds);
                qDebug() << read_timeout->tv_sec;
                rc = select(m_fd + 1, &read_fds, NULL, NULL, read_timeout);
                if ( rc < 0 ) {
                    // error
                    qWarning() << "select result < 0, error!";
                    assert(false);
                }
                else if ( rc == 0 ) {
                    qDebug() << "select timeout!";
                    ::free(read_timeout);
                    IndexGenerator::getInstance()->~IndexGenerator();
                   ::_exit(0);
                }else{
                    numRead = read(m_fd, buf, BUF_LEN);
                    if (numRead == -1){
                        printf("\033[1;31;40mread event error\033[0m\n");
                        fflush(stdout);
                        assert(false);
                    }
                    qDebug() << "Read " << numRead << " bytes from inotify fd";
                    this->eventProcess(buf, numRead);
                }
            }


//            QTimer* liveTime = new QTimer();
//            //restart inotify-index proccess per minute
//            liveTime->setInterval(60000);
//            liveTime->start();

            //I don't know how to use QTimer, wish someone can fix it!
            //MouseZhangZh

//            connect(liveTime, &QTimer::timeout, [ = ](){
////                ::_exit(0);
//                *b_timeout = 1;
//            });
//            for (;;){
//                qDebug() << "liveTime->remainingTime():" << liveTime->remainingTime();
//                numRead = read(m_fd, buf, BUF_LEN);
//                this->eventProcess(buf, numRead);
//                if (liveTime->remainingTime() < 1){
//                    qDebug() << "liveTime->remainingTime():" << liveTime->remainingTime();
//                    ::_exit(0);
//                }
//            }

        }
        else if (pid > 0){
            memset(buf, 0x00, BUF_LEN);
            waitpid(pid, NULL, 0);

            --FileUtils::_index_status;
        }
        else{
            assert(false);
        }
    }

}
