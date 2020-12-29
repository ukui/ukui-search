#include "file-reader.h"
#include "file-utils.h"

FileReader::FileReader(QObject *parent) : QObject(parent)
{

}

QString *FileReader::getTextContent(QString path)
{
    //获取所有文件内容
    //先分类
    QString type =FileUtils::getMimetype(path);
    if(type == "application/zip")
        return FileUtils::getDocxTextContent(path);
    else if(type == "text/plain")
        return FileUtils::getTxtContent(path);

    return new QString();
}
