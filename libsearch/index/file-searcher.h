#ifndef FILESEARCHER_H
#define FILESEARCHER_H

#include <QObject>
#include <xapian.h>
#include <QStandardPaths>
#include <QVector>
#include <QMap>
#include <QQueue>
#include <QPair>
#define INDEX_PATH (QStandardPaths::writableLocation(QStandardPaths::HomeLocation)+"/.config/org.ukui/index_data").toStdString()
#define CONTENT_INDEX_PATH (QStandardPaths::writableLocation(QStandardPaths::HomeLocation)+"/.config/org.ukui/content_index_data").toStdString()


class FileSearcher : public QObject
{
    Q_OBJECT
public:
    explicit FileSearcher(QObject *parent = nullptr);

public Q_SLOTS:
    void onKeywordSearch(QString keyword);

Q_SIGNALS:
    void resultFile(QQueue<QString> *);
    void resultDir(QQueue<QString> *);
    void resultContent(QQueue<QPair<QString,QStringList>> *);
private:
    int keywordSearchfile(QString keyword, QString value,unsigned slot = 1,int begin = 0, int num = 20);
    int keywordSearchContent(QString keyword, int begin = 0, int num = 20);

    /**
     * @brief FileSearcher::creatQueryForFileSearch
     * This part shall be optimized frequently to provide a more stable search function.
     * @param keyword
     * @param db
     * @return Xapian::Query
     */
    Xapian::Query creatQueryForFileSearch(QString keyword, Xapian::Database &db);
    Xapian::Query creatQueryForContentSearch(QString keyword, Xapian::Database &db);

    QStringList getResult(Xapian::MSet &result, QString value);
    QMap<QString,QStringList> getContentResult(Xapian::MSet &result,std::string &keyWord);

    bool isBlocked(QString &path);
    QQueue<QString> *m_search_result_file = nullptr;
    QQueue<QString> *m_search_result_dir = nullptr;
    QQueue<QPair<QString,QStringList>> *m_search_result_content = nullptr;

};

#endif // FILESEARCHER_H
