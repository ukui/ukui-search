#ifndef FILESEARCH_H
#define FILESEARCH_H

#include <QDir>
#include <QStringList>
#include <QDebug>
#include <QMap>
#include <QRegExp>
#include <src/Interface/ukuichineseletter.h>

class filesearch :public QObject
{
    Q_OBJECT

public:
    filesearch(QString searchInput);
    ~filesearch();
    QMap<QString,QStringList> returnResult();

private:
    int FindFile(const QString& _filePath);

private:
    QMap<QString,QStringList> searchResult; //返回结果
    QString test; //输入内容


};
#endif // FILESEARCH_H
