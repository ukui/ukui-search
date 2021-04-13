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
#ifndef INOTIFYINDEX_H
#define INOTIFYINDEX_H

#include <QThread>
#include <QTimer>
#include <unistd.h>
#include <sys/inotify.h>
#include "index-generator.h"
#include "traverse_bfs.h"
#include "ukui-search-qdbus.h"
#include "global-settings.h"
#include "file-utils.h"
#include "first-index.h"

#define BUF_LEN 1024000
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

    void eventProcess(const char*, ssize_t);
    void firstTraverse();
protected:
    void run() override;
private:
    QString* m_watch_path;
    int m_fd;

    QMap<int, QString> currentPath;

    const QMap<QString, bool> targetFileTypeMap = {
        std::map<QString, bool>::value_type("doc", true),
        std::map<QString, bool>::value_type("docx", true),
        std::map<QString, bool>::value_type("ppt", true),
        std::map<QString, bool>::value_type("pptx", true),
        std::map<QString, bool>::value_type("xls", true),
        std::map<QString, bool>::value_type("xlsx", true),
        std::map<QString, bool>::value_type("txt", true),
        std::map<QString, bool>::value_type("dot", true),
        std::map<QString, bool>::value_type("wps", true),
        std::map<QString, bool>::value_type("pps", true),
        std::map<QString, bool>::value_type("dps", true),
        std::map<QString, bool>::value_type("et", true)
    };

};

#endif // INOTIFYINDEX_H
