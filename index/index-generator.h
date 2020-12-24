#ifndef INDEXGENERATOR_H
#define INDEXGENERATOR_H

#include <xapian.h>
#include <QObject>

#include <QStringList>
#include <QMap>
#include <QCryptographicHash>

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
    bool creatAllIndex(QStringList *pathlist);
    bool deleteAllIndex(QStringList *pathlist);

private:
    explicit IndexGenerator(QObject *parent = nullptr);
    void HandlePathList(QStringList *pathlist);
    //add one data in database
    void insertIntoDatabase(QStringList *indexText,QString *doc);
    ~IndexGenerator();

    QMap<QString,QStringList> *m_index_map;
    QCryptographicHash *m_cryp;
    QString *m_index_data_path;
    Xapian::WritableDatabase *m_datebase;
    std::string m_docstr;
    std::string m_index_text_str;
    Xapian::TermGenerator *m_indexer;


};

#endif // INDEXGENERATOR_H
