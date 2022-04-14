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
#include <QQueue>
#include <QDesktopServices>
#include <QApplication>
#include <QClipboard>
#include <QFontMetrics>
#include <QLabel>

#include <quazip5/quazipfile.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <quazip5/quazip.h>
#include <uchardet/uchardet.h>
//#include <poppler-qt5.h>
#include <poppler/qt5/poppler-qt5.h>

#include "libsearch_global.h"
#include "common.h"
#include <leptonica/allheaders.h>

//#define INITIAL_STATE 0
//#define CREATING_INDEX 1
//#define FINISH_CREATING_INDEX 2
#define MAX_CONTENT_LENGTH 20480000
namespace UkuiSearch {
class  LIBSEARCH_EXPORT FileUtils {
public:
    static QString getHtmlText(const QString &text, const QString &keyword);
    static QString setAllTextBold(const QString &name);
    static QString wrapData(QLabel *p_label, const QString &text);
    static std::string makeDocUterm(QString path);
    static QIcon getFileIcon(const QString &uri, bool checkValid = true);
    static QIcon getAppIcon(const QString &path);
    static QIcon getSettingIcon(const QString &setting, const bool is_white);
    static QIcon getSettingIcon();

    static QString getFileName(const QString &uri);
    static QString getAppName(const QString &path);
    static QString getSettingName(const QString &setting);
    //A is or under B
    static bool isOrUnder(QString pathA, QString pathB);

    //chinese character to pinyin
    static QMap<QString, QStringList> map_chinese2pinyin;
    static QString find(const QString &hanzi);
    static QStringList findMultiToneWords(const QString &hanzi);
    static void loadHanziTable(const QString &fileName);

    //parse text,docx.....
    static QMimeType getMimetype(QString &path);
    static void getDocxTextContent(QString &path, QString &textcontent);
    static void getPptxTextContent(QString &path, QString &textcontent);
    static void getXlsxTextContent(QString &path, QString &textcontent);
    static void getPdfTextContent(QString &path, QString &textcontent);
    static void getTxtContent(QString &path, QString &textcontent);

    static int openFile(QString &path, bool openInDir = false);
    static bool copyPath(QString &path);
    static QString escapeHtml(const QString &str);
    static QString chineseSubString(const std::string &myStr,int start,int length);
    static QIcon iconFromTheme(const QString &name, const QIcon &iconDefault);
    static bool isOpenXMLFileEncrypted(QString &path);
    static bool isEncrypedOrUnreadable(QString path);
    static bool isOcrSupportSize(QString path);
    static size_t maxIndexCount;
    static unsigned short indexStatus;

    enum class SearchMethod { DIRECTSEARCH = 0, INDEXSEARCH = 1};
    static SearchMethod searchMethod;

private:
    FileUtils();
};

}

#endif // FILEUTILS_H
