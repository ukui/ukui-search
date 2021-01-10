#include <QDebug>
#include "filetypefilter.h"
#include "index-generator.h"

FileTypeFilter::FileTypeFilter(const QString& path) : Traverse_BFS(path)
{
    this->result = new QList<QString>();
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
            qDebug() << fileInfo.fileName();
            this->result->append(fileInfo.absoluteFilePath());
        }
    }

}

QList<QString>* FileTypeFilter::getTargetFileAbsolutePath(){
    return this->result;
}


void FileTypeFilter::Test(){
    IndexGenerator* ig = IndexGenerator::getInstance();
//    this->result = new QList<QString>();
//    this->result->append(QString("/home/zpf/桌面/DOCX 文档(1).docx"));

//        ig->creatAllIndex(this->result);
}


