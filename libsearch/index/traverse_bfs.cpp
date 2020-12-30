#include "traverse_bfs.h"

Traverse_BFS::Traverse_BFS(const QString& path)
{
    this->path = path;
}

void Traverse_BFS::Traverse(){
    QQueue<QString> bfs;
    bfs.enqueue(this->path);
    QFileInfoList list;
    QDir dir;
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::DirsFirst);
    while (!bfs.empty()) {
        dir.setPath(bfs.dequeue());
        list = dir.entryInfoList();
        for (auto i : list){
            if (i.isDir()){
                bfs.enqueue(i.absoluteFilePath());
            }
            DoSomething(i);
        }
    }
}

void Traverse_BFS::setPath(const QString& path){
    this->path = path;
}
