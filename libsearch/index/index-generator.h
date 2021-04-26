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
#include "document.h"
#include "file-reader.h"

extern QList<Document> *_doc_list_path;
extern QMutex  _mutex_doc_list_path;
extern QList<Document> *_doc_list_content;
extern QMutex  _mutex_doc_list_content;

class IndexGenerator : public QObject {
    Q_OBJECT
public:
    static IndexGenerator *getInstance(bool rebuild = false, QObject *parent = nullptr);
    ~IndexGenerator();
    bool setIndexdataPath();
    bool isIndexdataExist();
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
    bool deleteAllIndex(QStringList *pathlist);

private:
    explicit IndexGenerator(bool rebuild = false, QObject *parent = nullptr);
    static QMutex m_mutex;
    //For file name index
    void HandlePathList(QQueue<QVector<QString> > *messageList);
    //For file content index
    void HandlePathList(QQueue<QString> *messageList);
    static Document GenerateDocument(const QVector<QString> &list);
    static Document GenerateContentDocument(const QString &list);
    //add one data in database
    void insertIntoDatabase(Document& doc);
    void insertIntoContentDatabase(Document& doc);

//    QList<Document> *m_doc_list_path;  //for path index
//    QList<Document> *m_doc_list_content;  // for text content index
    QMap<QString, QStringList> m_index_map;
    QString m_index_data_path;
    Xapian::WritableDatabase* m_database_path;
    Xapian::WritableDatabase* m_database_content;
    std::string m_docstr;
    std::string m_index_text_str;
    Xapian::TermGenerator m_indexer;
};

#endif // INDEXGENERATOR_H
