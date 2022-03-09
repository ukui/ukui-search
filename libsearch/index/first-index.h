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
#include <QSystemSemaphore>
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
#include "traverse_bfs.h"
#include "index-status-recorder.h"
#include "index-generator.h"
#include "file-utils.h"
#include "common.h"
namespace Zeeker {
class FirstIndex : public QThread, public Traverse_BFS {
public:
    static FirstIndex* getInstance();
    ~FirstIndex();
    virtual void DoSomething(const QFileInfo &) final;
protected:
    void run() override;
private:
    FirstIndex();
    FirstIndex(const FirstIndex&) = delete;
    void operator=(const FirstIndex&) = delete;
    static FirstIndex *m_instance;
    bool bool_dataBaseStatusOK = false;
    bool bool_dataBaseExist = false;
    IndexGenerator* p_indexGenerator = nullptr;
    QThreadPool m_pool;

    QQueue<QVector<QString>>* q_index;
//    QQueue<QString>* q_content_index;
    //修改QQueue存储数据为QPair<QString,qint64>，增加存储文件大小数据便于处理时统计--jxx20210519
    QQueue<QPair<QString,qint64>>* q_content_index;
    //新增ocr队列存储ocr可识别处理的图片信息及大小；
    QQueue<QPair<QString,qint64>>* m_ocr_index;
    //xapian will auto commit per 10,000 changes, donnot change it!!!
    const size_t u_send_length = 8192;
    QSystemSemaphore m_semaphore;

};
}

#endif // FIRSTINDEX_H
