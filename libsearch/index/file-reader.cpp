#include "file-reader.h"
#include "file-utils.h"

FileReader::FileReader(QObject *parent) : QObject(parent)
{

}

void FileReader::getTextContent(QString path, QString &textContent)
{
    //获取所有文件内容
    //先分类
    QString type =FileUtils::getMimetype(path,true);
    if(type == "application/zip")
         FileUtils::getDocxTextContent(path,textContent);
    else if(type == "text/plain")
         FileUtils::getTxtContent(path,textContent);

    return;
}
