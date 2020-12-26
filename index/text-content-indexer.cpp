#include "text-content-indexer.h"
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDebug>

TextContentIndexer::TextContentIndexer(QObject *parent) : QObject(parent)
{

}

void TextContentIndexer::creatContentdata()
{

}

void TextContentIndexer::setFileList(QStringList *filelist)
{
    m_file_list = filelist;
}

void TextContentIndexer::begin()
{

}

bool TextContentIndexer::getPlaintextFileContent(QString path)
{
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
        return false;

    QTextStream *stream = new QTextStream(&file);
    QString content = stream->readAll();
    qDebug()<<content;
    return true;

}
