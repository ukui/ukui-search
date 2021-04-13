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
#include "traverse_bfs.h"

Traverse_BFS::Traverse_BFS(const QString& path)
{
    Q_ASSERT('/' == path.at(0));
    this->path = path;
}

void Traverse_BFS::Traverse(){
    QQueue<QString> bfs;
    bfs.enqueue(this->path);
    QFileInfoList list;
    QDir dir;
    // QDir::Hidden
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::DirsFirst);
    while (!bfs.empty()) {
        dir.setPath(bfs.dequeue());
        list = dir.entryInfoList();
        for (auto i : list){
            if (i.isDir() && (!(i.isSymLink()))){
                bfs.enqueue(i.absoluteFilePath());
            }
            DoSomething(i);
        }
    }
}

void Traverse_BFS::setPath(const QString& path){
    this->path = path;
}
