#ifndef FILEUTILS_H
#define FILEUTILS_H
#include "gobject-template.h"
#include <QString>
#include <QCryptographicHash>
#include <QIcon>
#include <QMap>
#include "libsearch_global.h"

class  LIBSEARCH_EXPORT FileUtils
{
public:
    static std::string makeDocUterm(QString );
    static QIcon getFileIcon(const QString &, bool checkValid = true);
    static QIcon getAppIcon(const QString &);
    static QIcon getSettingIcon(const QString &, const bool&);

    static QString getFileName(const QString &);
    static QString getAppName(const QString &);
    static QString getSettingName(const QString &);

    //chinese character to pinyin
    static QMap<QString, QStringList> map_chinese2pinyin;
    static QString find(const QString&);
    static QStringList findMultiToneWords(const QString&);
    static void loadHanziTable(const QString&);

    //parse text,docx.....
    static QString getMimetype(QString &path, bool getsuffix = false);
    static QString *getDocxTextContent(QString &path);
    static QString *getTxtContent(QString &path);

private:
    FileUtils();
};

#endif // FILEUTILS_H
