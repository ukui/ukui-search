#include <QDebug>
#include "filetypefilter.h"

FileTypeFilter::FileTypeFilter(const QString& path) : Traverse_BFS(path)
{
    this->result = new QVector<QString>();
    this->Traverse();
}

FileTypeFilter::~FileTypeFilter()
{
    delete this->result;
    this->result = nullptr;
}

void FileTypeFilter::DoSomething(const QFileInfo& fileInfo){
//    QMimeDatabase qmd;
//    QMimeType qmt;
//    qmt = qmd.mimeTypeForFile(fileInfo.fileName());
//    qDebug() << qmt.preferredSuffix();
    for (auto i : this->targetFileTypeVec){
        if (fileInfo.fileName().endsWith(i)){
//            qDebug() << fileInfo.fileName();
            this->result->append(fileInfo.absoluteFilePath());
        }
    }

}

QVector<QString>* FileTypeFilter::getTargetFileAbsolutePath(){
    return this->result;
}

