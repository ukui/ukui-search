#ifndef INDEXGENERATOR_H
#define INDEXGENERATOR_H

#include <xapian.h>
#include <QObject>

#include <QStringList>
#include <QMap>
#include <QCryptographicHash>
#include "document.h"

class IndexGenerator : public QObject
{
    Q_OBJECT
public:
    static IndexGenerator *getInstance();
    bool setIndexdataPath();
    bool isIndexdataExist();
    static QStringList IndexSearch(QString indexText);
Q_SIGNALS:
    void transactionFinished();
    void searchFinish();
public Q_SLOTS:
    bool creatAllIndex(QList<QVector<QString>> *messageList);
    bool deleteAllIndex(QStringList *pathlist);

private:
    explicit IndexGenerator(QObject *parent = nullptr);
    void HandlePathList(QList<QVector<QString>> *messageList);
    static Document GenerateDocument(const QVector<QString> &list);
    //add one data in database
    void insertIntoDatabase(Document doc);
    ~IndexGenerator();

    QMap<QString,QStringList> *m_index_map;
    QList<Document> *m_doc_list;

    QCryptographicHash *m_cryp;
    QString *m_index_data_path;
    Xapian::WritableDatabase *m_datebase;
    std::string m_docstr;
    std::string m_index_text_str;
    Xapian::TermGenerator *m_indexer;


};

#endif // INDEXGENERATOR_H
