/*
 * Copyright (C) 2022, KylinSoft Co., Ltd.
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
 * Authors: iaom <zhangpengfei@kylinos.cn>
 *
 */

#include "file-system-watcher-private.h"
#include <sys/inotify.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <QDir>
#include <QQueue>
#include <QtConcurrentRun>

#include "ukui-search-qdbus.h"
#include "file-utils.h"
using namespace UkuiSearch;

FileSystemWatcherPrivate::FileSystemWatcherPrivate(FileSystemWatcher *parent) : q(parent)
{
    qDebug() << "setInotifyMaxUserWatches start";
    UkuiSearchQDBus usQDBus;
    usQDBus.setInotifyMaxUserWatches();
    qDebug() << "setInotifyMaxUserWatches end";

    init();
    m_pool = new QThreadPool;
}

FileSystemWatcherPrivate::~FileSystemWatcherPrivate()
{
    if(m_notifier) {
        delete m_notifier;
        m_notifier = nullptr;
    }
    if(m_pool) {
        delete m_pool;
        m_pool = nullptr;
    }
}

void FileSystemWatcherPrivate::traverse(QStringList pathList)
{
    QQueue<QString> queue;
    for(QString path : pathList) {
        addWatch(path);
        queue.enqueue(path);
    }
    if(!m_recursive) {
        return;
    }
    QFileInfoList list;
    QDir dir;
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::DirsFirst);
    while(!queue.empty()) {
        dir.setPath(queue.dequeue());
        list = dir.entryInfoList();
        for(auto i : list) {
            if(i.isDir() && (!(i.isSymLink()))) {
                queue.enqueue(i.absoluteFilePath());
                addWatch(i.absoluteFilePath());
            }
        }
    }
}

void FileSystemWatcherPrivate::addWatch(const QString &path)
{
    int wd = inotify_add_watch(m_inotifyFd, path.toStdString().c_str(), m_watchEvents | m_watchFlags);
    if(wd > 0) {
        m_watchPathHash[wd] = path;
    } else {
        qWarning() << "AddWatch error:" << path << strerror(errno);
        if (errno == ENOSPC) {
            qWarning() << "User limit reached. Count: " << m_watchPathHash.count();
        }
    }
}

void FileSystemWatcherPrivate::addWatch(const QStringList &pathList)
{
    QtConcurrent::run(m_pool, [ = ](){
        traverse(pathList);
    });
}

QStringList FileSystemWatcherPrivate::removeWatch(const QString &path)
{
    m_pool->waitForDone();
    QStringList paths;
    for(int wd : m_watchPathHash.keys()) {
        QString tmpPath = m_watchPathHash.value(wd);
        if(tmpPath.length() >= path.length()) {
            if(FileUtils::isOrUnder(tmpPath, path)) {
                //fix me:This function can be slow (O(n))
                paths.append(removeWatch(m_watchPathHash.key(path)));
            }
        }
    }

    return paths;
}

QString FileSystemWatcherPrivate::removeWatch(int wd)
{
    inotify_rm_watch(m_inotifyFd, wd);
    return m_watchPathHash.take(wd);
}

void FileSystemWatcherPrivate::init()
{
    if(m_inotifyFd < 0) {
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
                return;
            }
        }
        fcntl(m_inotifyFd, F_SETFD, FD_CLOEXEC);
        m_notifier = new QSocketNotifier(m_inotifyFd, QSocketNotifier::Read);
        QObject::connect(m_notifier, &QSocketNotifier::activated, q, &FileSystemWatcher::eventProcess);
    }
}

FileSystemWatcher::FileSystemWatcher(bool recursive, WatchEvents events, WatchFlags flags, QObject *parent)
                  : QObject(parent)
                  , d(new FileSystemWatcherPrivate(this))
{
    d->m_watchEvents = events;
    d->m_watchFlags = flags;
    d->m_recursive = recursive;
}

FileSystemWatcher::~FileSystemWatcher()
{
    if(d) {
        delete d;
        d = nullptr;
    }
}

void FileSystemWatcher::addWatch(const QStringList &pathList)
{
    d->addWatch(pathList);
}

void FileSystemWatcher::addWatch(const QString &path)
{
    d->addWatch(QStringList(path));
}

QStringList FileSystemWatcher::removeWatch(const QString &path)
{
    return d->removeWatch(path);
}

void FileSystemWatcher::eventProcess(int socket)
{
    qDebug() << "-----begin event process-----";
    int avail;
    if (ioctl(socket, FIONREAD, &avail) == EINVAL) {
        qWarning() << "Did not receive an entire inotify event.";
        return;
    }

    char* buf = (char*)malloc(avail);
    memset(buf, 0x00, avail);

    const ssize_t len = read(socket, buf, avail);

    if(len != avail) {
        qWarning()<<"read event error";
    }

    int i = 0;
    while (i < len) {
        const struct inotify_event* event = (struct inotify_event*)&buf[i];

        if(event->name[0] == '.') {
            continue;
        }
        if (event->wd < 0 && (event->mask & EventQueueOverflow)) {
            qWarning() << "Inotify Event queued overflowed";
            free(buf);
            return;
        }

        qDebug() << "event mask:" << event->mask;
        QString path;

        if (event->mask & (EventDeleteSelf | EventMoveSelf)) {
            path = d->m_watchPathHash.value(event->wd);
        } else {
            path = d->m_watchPathHash[event->wd] + '/' + event->name;
        }

        if(event->mask & EventCreate) {
            qDebug() << path << "--EventCreate";
            Q_EMIT created(path, event->mask & IN_ISDIR);
            if(event->mask & IN_ISDIR && d->m_recursive) {
                if(!QFileInfo(path).isSymLink()){
                    addWatch(QStringList(path));
                }
            }
        }
        if (event->mask & EventDeleteSelf) {
            qDebug() << path << "--EventDeleteSelf";
            d->removeWatch(event->wd);
            Q_EMIT deleted(path, true);
        }
        if (event->mask & EventDelete) {
            qDebug() << path << "--EventDelete";
            // we watch all folders recursively. Thus, folder removing is reported in DeleteSelf.
            if (!(event->mask & IN_ISDIR)) {
                Q_EMIT deleted(path, false);
            }
        }
        if (event->mask & EventModify) {
            qDebug() << path << "--EventModify";
            Q_EMIT modified(path);
        }
        if (event->mask & EventMoveSelf) {
            qDebug() << path << "--EventMoveSelf";
            d->removeWatch(event->wd);
            Q_EMIT moved(path, true);
        }
        if (event->mask & EventMoveFrom) {
            qDebug() << path << "--EventMoveFrom";
            if (!(event->mask & IN_ISDIR)) {
                Q_EMIT moved(path, false);
            }
        }
        if (event->mask & EventMoveTo) {
            qDebug() << path << "--EventMoveTo";
            Q_EMIT created(path, event->mask & IN_ISDIR);
            if (event->mask & IN_ISDIR && d->m_recursive) {
                if(!QFileInfo(path).isSymLink()){
                    addWatch(QStringList(path));
                }
            }
        }
        if (event->mask & EventOpen) {
            qDebug() << path << "--EventOpen";
            Q_EMIT opened(path);
        }
        if (event->mask & EventUnmount) {
            qDebug() << path << "--EventUnmount";
            if (event->mask & IN_ISDIR) {
                d->removeWatch(event->wd);
            }
            // This is present because a unmount event is sent by inotify after unmounting, by
            // which time the watches have already been removed.
            if (path != "/") {
                Q_EMIT unmounted(path);
            }
        }
        if (event->mask & EventAttributeChange) {
            qDebug() << path << "--EventAttributeChange";
            Q_EMIT attributeChanged(path);
        }
        if (event->mask & EventAccess) {
            qDebug() << path << "--EventAccess";
            Q_EMIT accessed(path);
        }
        if (event->mask & EventCloseWrite) {
            qDebug() << path << "--EventCloseWrite";
            Q_EMIT closedWrite(path);
        }
        if (event->mask & EventCloseRead) {
            qDebug() << path << "--EventCloseRead";
            Q_EMIT closedRead(path);
        }
        if (event->mask & EventIgnored) {
            qDebug() << path << "--EventIgnored";
        }
        i += sizeof(struct inotify_event) + event->len;
    }
    if (len < 0) {
        qWarning() << "Failed to read event.";
    }

    free(buf);
}
