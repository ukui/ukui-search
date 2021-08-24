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
 *
 */
#ifndef SEARCHMANAGER_H
#define SEARCHMANAGER_H

#include <QObject>
#include <xapian.h>
#include <QStandardPaths>
#include <QVector>
#include <QMap>
#include <QQueue>
#include <QPair>
#include <QMutex>
#include <thread>
#include <QRunnable>
#include <QThreadPool>
#include <QFileInfo>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>
#include <QThread>
#include <QUrl>

#include "file-utils.h"
#include "global-settings.h"
#include "chinese-segmentation.h"


#define INDEX_PATH (QStandardPaths::writableLocation(QStandardPaths::HomeLocation)+"/.config/org.ukui/ukui-search/index_data").toStdString()
#define CONTENT_INDEX_PATH (QStandardPaths::writableLocation(QStandardPaths::HomeLocation)+"/.config/org.ukui/ukui-search/content_index_data").toStdString()

namespace Zeeker {

class LIBSEARCH_EXPORT SearchManager : public QObject {
    friend class FileSearch;
    friend class FileContentSearch;
    Q_OBJECT
public:
    explicit SearchManager(QObject *parent = nullptr);
    ~SearchManager();

    static int getCurrentIndexCount();

    static size_t uniqueSymbolFile;
    static size_t uniqueSymbolDir;
    static size_t uniqueSymbolContent;
    static QMutex m_mutexFile;
    static QMutex m_mutexDir;
    static QMutex m_mutexContent;

public Q_SLOTS:
    void onKeywordSearch(QString keyword, QQueue<QString> *searchResultFile, QQueue<QString> *searchResultDir, QQueue<QPair<QString, QStringList>> *searchResultContent);

Q_SIGNALS:
    void resultFile(QQueue<QString> *);
    void resultDir(QQueue<QString> *);
    void resultContent(QQueue<QPair<QString, QStringList>> *);
private:
//    int keywordSearchfile(size_t uniqueSymbol, QString keyword, QString value,unsigned slot = 1,int begin = 0, int num = 20);
//    int keywordSearchContent(size_t uniqueSymbol, QString keyword, int begin = 0, int num = 20);

    /**
     * @brief SearchManager::creatQueryForFileSearch
     * This part shall be optimized frequently to provide a more stable search function.
     * @param keyword
     * @param db
     * @return Xapian::Query
     */
//    Xapian::Query creatQueryForFileSearch(QString keyword, Xapian::Database &db);
//    Xapian::Query creatQueryForContentSearch(QString keyword, Xapian::Database &db);

//    int getResult(size_t uniqueSymbol, Xapian::MSet &result, QString value);
//    int getContentResult(size_t uniqueSymbol, Xapian::MSet &result,std::string &keyWord);

    static bool isBlocked(QString &path);

//    QQueue<QString> *m_search_result_file = nullptr;
//    QQueue<QString> *m_search_result_dir = nullptr;
//    QQueue<QPair<QString,QStringList>> *m_search_result_content = nullptr;
    QThreadPool m_pool;
};

class FileSearch : public QRunnable {
public:
    explicit FileSearch(QQueue<QString> *searchResult, size_t uniqueSymbol, QString keyword, QString value, unsigned slot = 1, int begin = 0, int num = 20);
    ~FileSearch();
protected:
    void run();
private:
    int keywordSearchfile();
    Xapian::Query creatQueryForFileSearch(Xapian::Database &db);
    int getResult(Xapian::MSet &result);

    QQueue<QString> *m_search_result = nullptr;
    QString m_value;
    unsigned m_slot = 1;
    size_t m_uniqueSymbol;
    QString m_keyword;
    int m_begin = 0;
    int m_num = 20;
};

class FileContentSearch : public QRunnable {
public:
    explicit FileContentSearch(QQueue<QPair<QString, QStringList>> *searchResult, size_t uniqueSymbol, QString keyword, int begin = 0, int num = 20);
    ~FileContentSearch();
protected:
    void run();
private:
    int keywordSearchContent();
    int getResult(Xapian::MSet &result, std::string &keyWord);

    QQueue<QPair<QString, QStringList>> *m_search_result = nullptr;
    size_t m_uniqueSymbol;
    QString m_keyword;
    int m_begin = 0;
    int m_num = 20;
};

class DirectSearch : public QRunnable {
public:
    explicit DirectSearch(QString keyword, QQueue<QString> *searchResultFile, QQueue<QString> *searchResultDir, size_t uniqueSymbol);
protected:
    void run();
private:
    QString m_keyword;
    QQueue<QString>* m_searchResultFile = nullptr;
    QQueue<QString>* m_searchResultDir = nullptr;
    size_t m_uniqueSymbol;
};

}
#endif // SEARCHMANAGER_H
