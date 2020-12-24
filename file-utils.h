#ifndef FILEUTILS_H
#define FILEUTILS_H
#include "gobject-template.h"
#include <QString>
#include <QCryptographicHash>
#include <QIcon>

class  FileUtils
{
public:
    static std::string makeDocUterm(QString *);

    static QIcon getFileIcon(const QString &, bool checkValid = true);
    static QIcon getAppIcon(const QString &);
    static QIcon getSettingIcon(const QString &);

    static QString getFileName(const QString &);
    static QString getAppName(const QString &);
    static QString getSettingName(const QString &);
private:
    FileUtils();
};

#endif // FILEUTILS_H
