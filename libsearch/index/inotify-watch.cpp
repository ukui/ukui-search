#include "inotify-watch.h"
#include <sys/ioctl.h>
#include <malloc.h>
#include <errno.h>
using namespace Zeeker;
static InotifyWatch* global_instance_InotifyWatch = nullptr;

Zeeker::InotifyWatch *Zeeker::InotifyWatch::getInstance(const QString &path)
{
    if(!global_instance_InotifyWatch) {
        global_instance_InotifyWatch = new InotifyWatch(path);
    }
    return global_instance_InotifyWatch;
}

Zeeker::InotifyWatch::InotifyWatch(const QString &path): Traverse_BFS(path)
{
    qDebug() << "setInotifyMaxUserWatches start";
    UkuiSearchQDBus usQDBus;
    usQDBus.setInotifyMaxUserWatches();
    qDebug() << "setInotifyMaxUserWatches end";
    m_sharedMemory = new QSharedMemory("ukui-search-shared-map", this);
}

InotifyWatch::~InotifyWatch()
{
    if(m_notifier)
        delete m_notifier;
    m_notifier = nullptr;
}

bool InotifyWatch::addWatch(const QString &path)
{
    int ret = inotify_add_watch(m_inotifyFd, path.toStdString().c_str(), (IN_MOVED_FROM | IN_MOVED_TO | IN_CREATE | IN_DELETE | IN_MODIFY));
    if(ret == -1) {
        qWarning() << "AddWatch error:" << path;
        return false;
    }
    currentPath[ret] = path;
//    qDebug() << "Watch: " << path << "ret: " << ret;
    return true;
}

bool InotifyWatch::removeWatch(const QString &path, bool removeFromDatabase)
{
    inotify_rm_watch(m_inotifyFd, currentPath.key(path));

    if(removeFromDatabase) {
        for(QMap<int, QString>::Iterator i = currentPath.begin(); i != currentPath.end();) {
            //        qDebug() << i.value();
            //            if(i.value().length() > path.length()) {
            if(FileUtils::isOrUnder(i.value(), path)) {
                qDebug() << "remove path: " << i.value();
                inotify_rm_watch(m_inotifyFd, currentPath.key(path));
                PendingFile f(i.value());
                f.setDeleted();
                f.setIsDir();
                PendingFileQueue::getInstance()->enqueue(f);
                currentPath.erase(i++);
            } else {
                i++;
            }
        }
    } else {
        for(QMap<int, QString>::Iterator i = currentPath.begin(); i != currentPath.end();) {
            //        qDebug() << i.value();
            if(i.value().length() > path.length()) {
                if(FileUtils::isOrUnder(i.value(), path)) {
//                if(i.value().startsWith(path + "/")) {
//                    qDebug() << "remove path: " << i.value();
                    inotify_rm_watch(m_inotifyFd, currentPath.key(path));
                    currentPath.erase(i++);
                } else {
                    i++;
                }
            } else {
                i++;
            }
        }
    }
    currentPath.remove(currentPath.key(path));
    return true;
}

void InotifyWatch::DoSomething(const QFileInfo &info)
{
    qDebug() << info.fileName() << "-------" << info.absoluteFilePath();
    if(info.isDir() && (!info.isSymLink())) {
        this->addWatch(info.absoluteFilePath());
    }
    PendingFile f(info.absoluteFilePath());
    if(info.isDir()) {
        f.setIsDir();
    }
    PendingFileQueue::getInstance()->enqueue(f);
}

void InotifyWatch::firstTraverse()
{
    QQueue<QString> bfs;
    bfs.enqueue(this->path);
    QFileInfoList list;
    QDir dir;
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::DirsFirst);
    while(!bfs.empty()) {
        dir.setPath(bfs.dequeue());
        list = dir.entryInfoList();
        for(auto i : list) {
            if(i.isDir() && (!(i.isSymLink()))) {
                this->addWatch(i.absoluteFilePath());
                bfs.enqueue(i.absoluteFilePath());
            }
        }
    }
}

void InotifyWatch::stopWatch()
{
//    if(this->isRunning()) {
//        this->quit();
//        if(m_notifier)
//            delete m_notifier;
//        m_notifier = nullptr;
//        removeWatch(QStandardPaths::writableLocation(QStandardPaths::HomeLocation), false);
//    }

//    IndexStatusRecorder::getInstance()->setStatus(INOTIFY_NORMAL_EXIT, "3");
}

void InotifyWatch::run()
{
    m_inotifyFd = inotify_init();
    if (m_inotifyFd > 0) {
        qDebug()<<"Inotify init success!";
    } else {
        qWarning() << "Inotify init fail! Now try add inotify_user_instances.";
        UkuiSearchQDBus usQDBus;
        usQDBus.addInotifyUserInstances(128);
        m_inotifyFd = inotify_init();
        if (m_inotifyFd > 0) {
            qDebug()<<"Inotify init success!";
        } else {
            printf("errno=%d\n",errno);
            printf("Mesg:%s\n",strerror(errno));
            Q_ASSERT_X(0, "InotifyWatch", "Failed to initialize inotify");
        }
    }

    this->addWatch(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    this->setPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    this->firstTraverse();

    int fifo_fd;
    char buffer[2];
    memset(buffer, 0, sizeof(buffer));
    fifo_fd = open(UKUI_SEARCH_PIPE_PATH, O_RDWR);
    if(fifo_fd == -1) {
        qWarning() << "Open fifo error\n";
        assert(false);
    }
    int retval = read(fifo_fd, buffer, sizeof(buffer));
    if(retval == -1) {
        qWarning() << "read error\n";
        assert(false);
    }
    qDebug("Read fifo[%s]", buffer);

    qDebug("Read data ok");
    close(fifo_fd);
    if(buffer[0] & 0x1) {
        qDebug("Data confirmed\n");
    }
    unlink(UKUI_SEARCH_PIPE_PATH);

    while(FileUtils::SearchMethod::INDEXSEARCH == FileUtils::searchMethod) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(m_inotifyFd, &fds);
        int rc;
        rc = select(m_inotifyFd + 1, &fds, NULL, NULL, NULL);
        if(rc > 0) {
            ++FileUtils::indexStatus;
            int avail;
            if (ioctl(m_inotifyFd, FIONREAD, &avail) == EINVAL) {
                qWarning() << "Did not receive an entire inotify event.";
                --FileUtils::indexStatus;
                return;
            }

            char* buf = (char*)malloc(avail);
            memset(buf, 0x00, avail);

            const ssize_t len = read(m_inotifyFd, buf, avail);
            if(len != avail) {
                qWarning()<<"read event error";
        //        IndexStatusRecorder::getInstance()->setStatus(INOTIFY_NORMAL_EXIT, "1");
            }

            int i = 0;
            while (i < len) {
                const struct inotify_event* event = (struct inotify_event*)&buf[i];
                if(event->name[0] != '.') {
                    //                qDebug() << "Read Event: " << currentPath[event->wd] << QString(event->name) << event->cookie << event->wd << event->mask;
                    //                qDebug("mask:0x%x,",event->mask);
                    break;
                }
                i += sizeof(struct inotify_event) + event->len;
            }
            if(i < len ) {
                qDebug() << "fork";
                slotEvent(buf, len);
                free(buf);
            }
            --FileUtils::indexStatus;
        } else if(rc < 0) {
            // error
            qWarning() << "select result < 0, error!";
            IndexStatusRecorder::getInstance()->setStatus(INOTIFY_NORMAL_EXIT, "1");
            assert(false);
        }
    }
    qDebug() << "Leave watch loop";
    if(FileUtils::SearchMethod::DIRECTSEARCH == FileUtils::searchMethod) {
        IndexStatusRecorder::getInstance()->setStatus(INOTIFY_NORMAL_EXIT, "3");
        removeWatch(QStandardPaths::writableLocation(QStandardPaths::HomeLocation), false);
    }
    close(m_inotifyFd);
//    fcntl(m_inotifyFd, F_SETFD, FD_CLOEXEC);
//    m_notifier = new QSocketNotifier(m_inotifyFd, QSocketNotifier::Read);
//    connect(m_notifier, &QSocketNotifier::activated, this, &InotifyWatch::slotEvent, Qt::DirectConnection);
//    exec();
}

void InotifyWatch::slotEvent(char *buf, ssize_t len)
{
//    eventProcess(socket);
    if(FileUtils::SearchMethod::INDEXSEARCH == FileUtils::searchMethod) {
        ++FileUtils::_index_status;
        pid_t pid;
        pid = fork();
        if(pid  == 0) {
            prctl(PR_SET_PDEATHSIG, SIGTERM);
            prctl(PR_SET_NAME, "inotify-index");
            this->eventProcess(buf, len);
            fd_set read_fds;
            int rc;
            timeval* read_timeout = (timeval*)malloc(sizeof(timeval));
            read_timeout->tv_sec = 40;
            read_timeout->tv_usec = 0;
            for(;;) {
                FD_ZERO(&read_fds);
                FD_SET(m_inotifyFd, &read_fds);
                rc = select(m_inotifyFd + 1, &read_fds, NULL, NULL, read_timeout);
                if(rc < 0) {
                    // error
                    qWarning() << "fork select result < 0, error!";
                    IndexStatusRecorder::getInstance()->setStatus(INOTIFY_NORMAL_EXIT, "1");
                    assert(false);
                } else if(rc == 0) {
                    qDebug() << "select timeout!";
                    ::free(read_timeout);

                    QBuffer buffer;
                    QDataStream out(&buffer);
                    if (m_sharedMemory->isAttached()) {
                        m_sharedMemory->detach();
                    }
                    buffer.open(QBuffer::ReadWrite);
                    out << currentPath;
                    int size = buffer.size();
                    if (!m_sharedMemory->create(size)) {
                        qDebug() << "Create sharedMemory Error: " << m_sharedMemory->errorString();
                    } else {
                        m_sharedMemory->lock();
                        char *to = static_cast<char *>(m_sharedMemory->data());
                        const char *from = buffer.data().constData();
                        memcpy(to, from, qMin(size, m_sharedMemory->size()));
                        m_sharedMemory->unlock();
                    }
                    //                    GlobalSettings::getInstance()->forceSync();
                    PendingFileQueue::getInstance()->forceFinish();
                    PendingFileQueue::getInstance()->~PendingFileQueue();
                    ::_exit(0);
                } else {
//                    qDebug() << "Select remain:" <<read_timeout->tv_sec;
                    this->eventProcess(m_inotifyFd);
//                    qDebug() << "Select remain:" <<read_timeout->tv_sec;
                }
            }
        } else if(pid > 0) {
            waitpid(pid, NULL, 0);
            if (!m_sharedMemory->attach()) {
                qDebug() << "SharedMemory attach Error: " << m_sharedMemory->errorString();
            } else {
                QBuffer buffer;
                QDataStream in(&buffer);
                QMap<int, QString> pathMap;
                m_sharedMemory->lock();
                buffer.setData(static_cast<const char *>(m_sharedMemory->constData()), m_sharedMemory->size());
                buffer.open(QBuffer::ReadWrite);
                in >> pathMap;
                m_sharedMemory->unlock();
                m_sharedMemory->detach();
                currentPath = pathMap;
            }
        } else {
            assert(false);
        }
    }
}

char * InotifyWatch::filter()
{
    int avail;
    if (ioctl(m_inotifyFd, FIONREAD, &avail) == EINVAL) {
        qWarning() << "Did not receive an entire inotify event.";
        return NULL;
    }

    char* buffer = (char*)malloc(avail);
    memset(buffer, 0x00, avail);

    const int len = read(m_inotifyFd, buffer, avail);
    if(len != avail) {
        qWarning()<<"read event error";
//        IndexStatusRecorder::getInstance()->setStatus(INOTIFY_NORMAL_EXIT, "1");
    }

    int i = 0;
    while (i < len) {
        const struct inotify_event* event = (struct inotify_event*)&buffer[i];
        if(event->name[0] == '.') {
            //                qDebug() << "Read Event: " << currentPath[event->wd] << QString(event->name) << event->cookie << event->wd << event->mask;
            //                qDebug("mask:0x%x,",event->mask);
            i += sizeof(struct inotify_event) + event->len;
            return buffer;
        }
    }
    free(buffer);
    return NULL;
}
void InotifyWatch::eventProcess(int socket)
{
//    qDebug()<< "Enter eventProcess!";
    int avail;
    if (ioctl(socket, FIONREAD, &avail) == EINVAL) {
        qWarning() << "Did not receive an entire inotify event.";
        return;
    }

    char* buffer = (char*)malloc(avail);
    memset(buffer, 0x00, avail);

    const ssize_t len = read(socket, buffer, avail);
    if(len != avail) {
        qWarning()<<"read event error";
//        IndexStatusRecorder::getInstance()->setStatus(INOTIFY_NORMAL_EXIT, "1");
    }
    int i = 0;
    while (i < len) {
        const struct inotify_event* event = (struct inotify_event*)&buffer[i];
        if(event->name[0] != '.') {
//            qDebug() << "Read Event: " << currentPath[event->wd] << QString(event->name) << event->cookie << event->wd << event->mask;
//            qDebug("mask:0x%x,",event->mask);
            break;
        }
        i += sizeof(struct inotify_event) + event->len;
    }
    if(i >= len) {
        qDebug() << "There is nothing to do!";
        return;
    }
    eventProcess(buffer, len);
    free(buffer);
}

void InotifyWatch::eventProcess(const char *buffer, ssize_t len)
{
//    qDebug()<< "Begin eventProcess! len:" << len;

    char * p = const_cast<char*>(buffer);
    while (p < buffer + len) {
        const struct inotify_event* event = reinterpret_cast<inotify_event *>(p);
//        qDebug() << "Read Event: " << currentPath[event->wd] << QString(event->name) << event->cookie << event->wd << event->mask;
//        qDebug("mask:0x%x,",event->mask);
        if(event->name[0] != '.') {
            QString path = currentPath[event->wd] + '/' + event->name;
            //Create top dir first, traverse it last.
            if(event->mask & IN_CREATE) {
//                qDebug() << "IN_CREATE";
                PendingFile f(path);
                if(event->mask & IN_ISDIR) {
                    f.setIsDir();
                }
                PendingFileQueue::getInstance(this)->enqueue(f);

                if(event->mask & IN_ISDIR) {
                    if(!QFileInfo(path).isSymLink()){
                        addWatch(path);
                        setPath(path);
                        Traverse();
                    }
                }
                goto next;

            }

            if((event->mask & IN_DELETE) | (event->mask & IN_MOVED_FROM)) {
                qDebug() << "IN_DELETE or IN_MOVED_FROM";
                if(event->mask & IN_ISDIR) {
                    removeWatch(path);
                } else {
                    PendingFile f(path);
                    f.setDeleted();
                    PendingFileQueue::getInstance()->enqueue(f);
                }
                p += sizeof(struct inotify_event) + event->len;
                continue;
            }
            if(event->mask & IN_MODIFY) {
//                qDebug() << "IN_MODIFY";
                if(!(event->mask & IN_ISDIR)) {
                    PendingFileQueue::getInstance()->enqueue(PendingFile(path));
                }
                goto next;

            }

            if(event->mask & IN_MOVED_TO) {
                qDebug() << "IN_MOVED_TO";
                if(event->mask & IN_ISDIR) {
                    removeWatch(path);

                    PendingFile f(path);
                    f.setIsDir();
                    PendingFileQueue::getInstance()->enqueue(f);

                    if(!QFileInfo(path).isSymLink()){
                        addWatch(path);
                        setPath(path);
                        Traverse();
                    }
                } else {
                    //Enqueue a deleted file to merge.
                    PendingFile f(path);
                    f.setDeleted();
                    PendingFileQueue::getInstance()->enqueue(f);
                    //Enqueue a new one.
                    PendingFileQueue::getInstance()->enqueue(PendingFile(path));
                }
                goto next;
            }
        }
next:
        p += sizeof(struct inotify_event) + event->len;
    }
//    qDebug()<< "Finish eventProcess!";
}


