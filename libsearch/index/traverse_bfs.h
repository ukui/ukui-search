#ifndef TRAVERSE_BFS_H
#define TRAVERSE_BFS_H


#include <QDebug>
#include <QDir>
#include <QQueue>

class Traverse_BFS
{
public:
    void Traverse();
    virtual ~Traverse_BFS() = default;
    virtual void DoSomething(const QFileInfo&) = 0;
    void setPath(const QString&);
protected:
    explicit Traverse_BFS(const QString&);
private:
    Traverse_BFS(const Traverse_BFS&) = delete;
    void operator=(const Traverse_BFS&) = delete;
    QString path = "/home";
};

#endif // TRAVERSE_BFS_H
