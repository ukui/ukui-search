#ifndef FILESEARCHER_H
#define FILESEARCHER_H

#include <QObject>
#include <xapian.h>
#define INDEX_PATH (QStandardPaths::writableLocation(QStandardPaths::HomeLocation)+"/.config/org.ukui/index_data").toStdString()


class FileSearcher : public QObject
{
    Q_OBJECT
public:
    explicit FileSearcher(QObject *parent = nullptr);

public Q_SLOTS:
    void onKeywordSearch(QString keyword);

Q_SIGNALS:
    void result(QStringList resultList);


};

#endif // FILESEARCHER_H
