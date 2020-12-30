#ifndef FILETYPEFILTER_H
#define FILETYPEFILTER_H

#include <QObject>
#include <QMimeDatabase>
#include <QMimeType>
#include <QVector>
#include "traverse_bfs.h"

class FileTypeFilter : public QObject, public Traverse_BFS
{
    Q_OBJECT
public:
    explicit FileTypeFilter(const QString&);
    ~FileTypeFilter();
    virtual void DoSomething(const QFileInfo&) final;
    QVector<QString>* getTargetFileAbsolutePath();

Q_SIGNALS:
private:
    const QVector<QString> targetFileTypeVec ={ QString(".doc"),
                                                QString(".docx"),
                                                QString(".ppt"),
                                                QString(".pptx"),
                                                QString(".xls"),
                                                QString(".xlsx"),
                                                QString(".txt")};
    QVector<QString>* result;

};

#endif // FILETYPEFILTER_H
