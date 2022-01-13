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
 * Authors: zhangpengfei <zhangpengfei@kylinos.cn>
 * Modified by: zhangzihao <zhangzihao@kylinos.cn>
 *
 */
#ifndef INOTIFYWATCH_H
#define INOTIFYWATCH_H

#include <QThread>
#include <unistd.h>
#include <sys/inotify.h>
#include <QSocketNotifier>
#include <QDataStream>
#include <QSharedMemory>

#include "traverse_bfs.h"
#include "ukui-search-qdbus.h"
#include "index-status-recorder.h"
#include "file-utils.h"
#include "first-index.h"
#include "pending-file-queue.h"
#include "common.h"
namespace Zeeker {
class InotifyWatch : public QThread, public Traverse_BFS
{
    Q_OBJECT
public:
    static InotifyWatch* getInstance(const QString& path);

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
    explicit InotifyWatch(const QString& path);
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
