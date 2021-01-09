//aborted --MouseZhangZh
#ifndef FILETYPEFILTER_H
#define FILETYPEFILTER_H

#include <QObject>
#include <QMimeDatabase>
#include <QMimeType>
#include <QList>
#include "traverse_bfs.h"

class FileTypeFilter : public QObject, public Traverse_BFS
{
    Q_OBJECT
public:
    explicit FileTypeFilter(const QString&);
    ~FileTypeFilter();
    virtual void DoSomething(const QFileInfo&) final;
    QList<QString>* getTargetFileAbsolutePath();
    void Test();
    void startIndexText();
Q_SIGNALS:
private:
    const QVector<QString> targetFileTypeVec ={
//        QString(".doc"),
        QString(".docx"),
//        QString(".ppt"),
//        QString(".pptx"),
//        QString(".xls"),
//        QString(".xlsx"),
        QString(".txt")};
    QList<QString>* result;

};

#endif // FILETYPEFILTER_H
