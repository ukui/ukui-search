#ifndef FILESEARCHER_H
#define FILESEARCHER_H

#include <QObject>
#include <xapian.h>
#include <QStandardPaths>
#include <QVector>
#include <QMap>
#define INDEX_PATH (QStandardPaths::writableLocation(QStandardPaths::HomeLocation)+"/.config/org.ukui/index_data").toStdString()
#define CONTENT_INDEX_PATH (QStandardPaths::writableLocation(QStandardPaths::HomeLocation)+"/.config/org.ukui/content_index_data").toStdString()


class FileSearcher : public QObject
{
    Q_OBJECT
public:
    explicit FileSearcher(QObject *parent = nullptr);

public Q_SLOTS:
    void onKeywordSearch(QString keyword, int begin = 0, int num = 20);
    void onKeywordSearchContent(QString keyword, int begin = 0, int num = 20);

Q_SIGNALS:
    void result(QVector<QStringList> resultP);
    void contentResult(QMap<QString,QStringList> resultC);
private:
    QStringList getResult(Xapian::MSet &result);
    QMap<QString,QStringList> getContentResult(Xapian::MSet &result,std::string &keyWord);
};

#endif // FILESEARCHER_H
