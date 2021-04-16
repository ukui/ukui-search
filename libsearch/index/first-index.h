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
 * Modified by: zhangpengfei <zhangpengfei@kylinos.cn>
 *
 */
#ifndef FIRSTINDEX_H
#define FIRSTINDEX_H

#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include <signal.h>
#include <QSemaphore>
#include<sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <syslog.h>
//#include <QtConcurrent>
#include "traverse_bfs.h"
#include "global-settings.h"
#include "index-generator.h"
#include "inotify-index.h"
#include "file-utils.h"

class FirstIndex : public QThread, public Traverse_BFS
{
public:
    FirstIndex(const QString&);
    ~FirstIndex();
    virtual void DoSomething(const QFileInfo &) final;
protected:
    void run() override;
private:
    FirstIndex(const FirstIndex&) = delete;
    void operator=(const FirstIndex&) = delete;
    bool bool_dataBaseStatusOK = false;
    bool bool_dataBaseExist = false;
    IndexGenerator* p_indexGenerator = nullptr;

    //here should be refact
//    MessageListManager* mlm;

    //test
    QQueue<QVector<QString>>* q_index;
    QQueue<QString>* q_content_index;

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
        std::map<QString, bool>::value_type("et", true),
        std::map<QString, bool>::value_type("pdf", true)
    };

    //xapian will auto commit per 10,000 changes, donnot change it!!!
    const size_t u_send_length = 8192;
};

#endif // FIRSTINDEX_H
