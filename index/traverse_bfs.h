#ifndef TRAVERSE_BFS_H
#define TRAVERSE_BFS_H


#include <QDebug>
#include <QDir>
#include <QQueue>

class Traverse_BFS
{
public:
    explicit Traverse_BFS(const QString&);
    void Traverse();
    virtual void DoSomething(const QFileInfo&) = 0;
    void setPath(const QString&);
private:
    QString path = "/home";
};

#endif // TRAVERSE_BFS_H
