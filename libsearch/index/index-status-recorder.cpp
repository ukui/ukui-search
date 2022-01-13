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
 * Authors: zhangpengfei <zhangpengfei@kylinos.cn>
 * Modified by: zhangzihao <zhangzihao@kylinos.cn>
 *
 */
#include "index-status-recorder.h"

using namespace Zeeker;
static IndexStatusRecorder *global_instance_indexStatusRecorder = nullptr;

IndexStatusRecorder *IndexStatusRecorder::getInstance()
{
    if(!global_instance_indexStatusRecorder) {
        global_instance_indexStatusRecorder = new IndexStatusRecorder;
    }
    return global_instance_indexStatusRecorder;
}

void IndexStatusRecorder::setStatus(const QString &key, const QVariant &value)
{
    m_mutex.lock();
    m_status->setValue(key, value);
    m_status->sync();
    m_mutex.unlock();
}

const QVariant IndexStatusRecorder::getStatus(const QString &key)
{
    return m_status->value(key);
}

IndexStatusRecorder::IndexStatusRecorder(QObject *parent) : QObject(parent)
{
    m_status = new QSettings(INDEX_STATUS, QSettings::IniFormat, this);
}
