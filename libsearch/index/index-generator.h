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
#ifndef INDEXGENERATOR_H
#define INDEXGENERATOR_H

#include <xapian.h>
#include <QObject>
//#include <QtConcurrent/QtConcurrent>
#include <QStringList>
#include <QMap>
#include <QCryptographicHash>
#include <QMutex>
#include <QQueue>
//#include <QMetaObject>
#include "construct-document.h"
#include "index-status-recorder.h"
#include "document.h"
#include "file-reader.h"
#include "common.h"
#include "pending-file.h"
#include "common.h"

namespace UkuiSearch {
//extern QVector<Document> *_doc_list_path;
//extern QMutex  _mutex_doc_list_path;
//extern QVector<Document> *_doc_list_content;
//extern QMutex  _mutex_doc_list_content;

class IndexGenerator : public QObject {
    friend class ConstructDocumentForPath;
    friend class ConstructDocumentForContent;
    friend class ConstructDocumentForOcr;
    Q_OBJECT
public:
    static IndexGenerator *getInstance();
    ~IndexGenerator();

    void rebuildIndexDatabase(const QString &path = INDEX_PATH);
    void rebuildContentIndexDatabase(const QString &path = CONTENT_INDEX_PATH);
    void rebuildOcrIndexDatabase(const QString &path = OCR_INDEX_PATH);
//    Q_INVOKABLE void appendDocListPath(Document doc);
    //for search test
    static QStringList IndexSearch(QString indexText);
    void setSynonym();
Q_SIGNALS:
    void transactionFinished();
    void searchFinish();
public Q_SLOTS:
    bool creatAllIndex(QQueue<QVector<QString>> *messageList);
    bool creatAllIndex(QQueue<QString> *messageList);
    bool creatOcrIndex(QQueue<QString> *messageList);
    bool deleteAllIndex(QStringList *pathlist);
    bool deleteContentIndex(QStringList *pathlist);
    bool deleteOcrIndex(QStringList *pathlist);
    bool updateIndex(QVector<PendingFile> *pendingFiles);

private:
    explicit IndexGenerator(QObject *parent = nullptr);
    static QMutex m_mutex;
    //For file name index
    void HandlePathList(QQueue<QVector<QString> > *messageList);
    //For file content index
    void HandlePathList(QQueue<QString> *messageList);
    //For ocr index
    void HandleOcrPathList(QQueue<QString> *messageList);
    static Document GenerateDocument(const QVector<QString> &list);
    static Document GenerateContentDocument(const QString &list);
    //add one data in database
    void insertIntoDatabase(Document& doc);
    void insertIntoContentDatabase(Document& doc);

    static QVector<Document> g_docListForPath;
    static QMutex  g_mutexDocListForPath;
    static QVector<Document> g_docListForContent;
    static QMutex  g_mutexDocListForContent;
    static QVector<Document> g_docListForOcr;
    static QMutex  g_mutexDocListForOcr;
    QMap<QString, QStringList> m_index_map;
    QString m_index_data_path;
    Xapian::WritableDatabase* m_database_path = nullptr;
    Xapian::WritableDatabase* m_database_content = nullptr;
    Xapian::WritableDatabase* m_database_ocr = nullptr;
    std::string m_docstr;
    std::string m_index_text_str;
    Xapian::TermGenerator m_indexer;
};
}

#endif // INDEXGENERATOR_H
