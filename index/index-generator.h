#ifndef INDEXGENERATOR_H
#define INDEXGENERATOR_H

#include <xapian.h>
#include <QObject>

#include <QStringList>
#include <QMultiMap>
#include <QCryptographicHash>

class IndexGenerator : public QObject
{
    Q_OBJECT
public:
    static IndexGenerator *getInstance();
    bool isIndexdataExist();
Q_SIGNALS:
    void transactionFinished();
    void searchFinish();
public Q_SLOTS:
    bool creatAllIndex(QStringList *pathlist);
    bool deleteAllIndex(QStringList *pathlist);
    void IndexSearch(QString *indexText);
private:
    explicit IndexGenerator(QObject *parent = nullptr);
    void HandlePathList(QStringList *pathlist);
    //add one data in database
    void CreatIndex(QString *indexText,QString *doc);
    ~IndexGenerator();

    QMap<QString,QString> *m_index_map;
    QCryptographicHash *m_cryp;
    QString *m_index_data_path;
    Xapian::WritableDatabase *m_datebase;
    std::string m_docstr;
    std::string m_index_text_str;
    Xapian::TermGenerator *m_indexer;


};

#endif // INDEXGENERATOR_H
