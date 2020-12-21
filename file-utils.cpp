#include "file-utils.h"

FileUtils::FileUtils()
{
}

std::string FileUtils::makeDocUterm(QString *path)
{
    return QCryptographicHash::hash(path->toUtf8(),QCryptographicHash::Md5).toStdString();
}
