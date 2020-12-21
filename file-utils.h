#ifndef FILEUTILS_H
#define FILEUTILS_H
#include <QString>
#include <QCryptographicHash>

class  FileUtils
{
public:
    static std::string makeDocUterm(QString *path);
private:
    FileUtils();
};

#endif // FILEUTILS_H
