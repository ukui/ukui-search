/*
 * Copyright (C) 2020, KylinSoft Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: zhangpengfei <zhangpengfei@kylinos.cn>
 * Modified by: zhangzihao <zhangzihao@kylinos.cn>
 * Modified by: zhangjiaping <zhangjiaping@kylinos.cn>
 *
 */
#ifndef FILEUTILS_H
#define FILEUTILS_H
#include <QString>
#include <QCryptographicHash>
#include <QIcon>
#include <QMap>
#include <QMimeDatabase>
#include <QMimeType>
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QUrl>
#include <QMap>
#include <QDomDocument>
#include <QQueue>
#include <QApplication>
#include <QFontMetrics>

#include <quazip/quazipfile.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <quazip/quazip.h>
#include <uchardet/uchardet.h>
//#include <poppler-qt5.h>
#include <poppler/qt5/poppler-qt5.h>

#include "libsearch_global.h"
#include "gobject-template.h"

//#define INITIAL_STATE 0
//#define CREATING_INDEX 1
//#define FINISH_CREATING_INDEX 2
#define MAX_CONTENT_LENGTH 20480000

namespace Zeeker {
class  LIBSEARCH_EXPORT FileUtils {
public:
    static std::string makeDocUterm(QString);
    static QIcon getFileIcon(const QString &, bool checkValid = true);
    static QIcon getAppIcon(const QString &);
    static QIcon getSettingIcon(const QString &, const bool&);

    static QString getFileName(const QString &);
    static QString getAppName(const QString &);
    static QString getSettingName(const QString &);
    //A is or under B
    static bool isOrUnder(QString pathA, QString pathB);

    //chinese character to pinyin
    static QMap<QString, QStringList> map_chinese2pinyin;
    static QString find(const QString&);
    static QStringList findMultiToneWords(const QString&);
    static void loadHanziTable(const QString&);

    //parse text,docx.....
    static QMimeType getMimetype(QString &path);
    static void getDocxTextContent(QString &path, QString &textcontent);
    static void getPptxTextContent(QString &path, QString &textcontent);
    static void getXlsxTextContent(QString &path, QString &textcontent);
    static void getPdfTextContent(QString &path, QString &textcontent);
    static void getTxtContent(QString &path, QString &textcontent);
    static QString chineseSubString(const std::string &data, int start, int length);
    static size_t _max_index_count;
    static size_t _current_index_count; //this one has been Abandoned,do not use it.
    static unsigned short _index_status;

    enum class SearchMethod { DIRECTSEARCH = 0, INDEXSEARCH = 1};
    static SearchMethod searchMethod;

private:
    FileUtils();
};

}

#endif // FILEUTILS_H
