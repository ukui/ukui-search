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
#ifndef INDEXSTATUSRECORDER_H
#define INDEXSTATUSRECORDER_H

#include <QObject>
#include <QSettings>
#include <QDir>
#include <QMutex>
#define CONTENT_INDEX_DATABASE_STATE "content_index_database_state"
#define INDEX_DATABASE_STATE "index_database_state"
#define INOTIFY_NORMAL_EXIT "inotify_normal_exit"
#define PENDING_FILE_QUEUE_FINISH "pending_file_queue_finish"
#define INDEX_STATUS QDir::homePath() + "/.config/org.ukui/ukui-search/ukui-search-index-status.conf"
namespace Zeeker {
//fixme: we need a better way to record index status.
class IndexStatusRecorder : public QObject
{
    Q_OBJECT
public:
    static IndexStatusRecorder *getInstance();
    void setStatus(const QString& key, const QVariant& value);
    const QVariant getStatus(const QString& key);

private:
    explicit IndexStatusRecorder(QObject *parent = nullptr);
    QSettings *m_status;
    QMutex m_mutex;
};
}

#endif // INDEXSTATUSRECORDER_H
