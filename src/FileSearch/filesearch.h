#ifndef FILESEARCH_H
#define FILESEARCH_H

#include <QDir>
#include <QStringList>
#include <QDebug>
#include <QMap>
#include <QRegExp>

class filesearch :public QObject
{
    Q_OBJECT

public:
    filesearch(QString searchInput);
    ~filesearch();
    int FindFile(const QString& _filePath);
    QMap<QString,QString> searchResult;
    QString test;
    QMap<QString,QString> returnResult();

};
#endif // FILESEARCH_H
