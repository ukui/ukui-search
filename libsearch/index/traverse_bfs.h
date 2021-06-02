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
#ifndef TRAVERSE_BFS_H
#define TRAVERSE_BFS_H


#include <QDebug>
#include <QDir>
#include <QQueue>
namespace Zeeker {
class Traverse_BFS {
public:
    Traverse_BFS() = default;
    void Traverse();
    virtual ~Traverse_BFS() = default;
    virtual void DoSomething(const QFileInfo&) = 0;
    void setPath(const QStringList &pathList);
protected:
    Traverse_BFS(const QStringList &pathList);
    QStringList m_pathList;
private:
    Traverse_BFS(const Traverse_BFS&) = delete;
    void operator=(const Traverse_BFS&) = delete;
};
}

#endif // TRAVERSE_BFS_H
