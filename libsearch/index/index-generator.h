#ifndef INDEXGENERATOR_H
#define INDEXGENERATOR_H

#include <xapian.h>
#include <QObject>
//#include <QtConcurrent/QtConcurrent>
#include <QStringList>
#include <QMap>
#include <QCryptographicHash>
#include "document.h"
#include "file-reader.h"

class IndexGenerator : public QObject
{
    Q_OBJECT
public:
    static IndexGenerator *getInstance(bool rebuild = false);
    ~IndexGenerator();
    bool setIndexdataPath();
    bool isIndexdataExist();
    //for search test
    static QStringList IndexSearch(QString indexText);
Q_SIGNALS:
    void transactionFinished();
    void searchFinish();
public Q_SLOTS:
    bool creatAllIndex(QList<QVector<QString>> *messageList);
    bool creatAllIndex(QList<QString> *messageList);
    bool deleteAllIndex(QStringList *pathlist);

private:
    explicit IndexGenerator(bool rebuild = false,QObject *parent = nullptr);
    //For file name index
    void HandlePathList(QList<QVector<QString>> *messageList);
    //For file content index
    void HandlePathList(QList<QString> *messageList);
    static Document GenerateDocument(const QVector<QString> &list);
    static Document GenerateContentDocument(const QString &list);
    //add one data in database
    void insertIntoDatabase(Document doc);
    void insertIntoContentDatabase(Document doc);

    QMap<QString,QStringList> *m_index_map;
    QList<Document> *m_doc_list_path;  //for path index
    QList<Document> *m_doc_list_content;  // for text content index
    QString *m_index_data_path;
    Xapian::WritableDatabase *m_datebase_path;
    Xapian::WritableDatabase *m_database_content;
    std::string m_docstr;
    std::string m_index_text_str;
    Xapian::TermGenerator *m_indexer;
};

#endif // INDEXGENERATOR_H
