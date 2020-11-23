#ifndef SEARCHFILETHREAD_H
#define SEARCHFILETHREAD_H

#include <QThread>
#include <QDebug>
#include "filesearch.h"
class SearchFileThread: public QThread
{
    Q_OBJECT
public:
    SearchFileThread();
    ~SearchFileThread();
    void run();
    QString mSearchline;
    QStringList returnResult;

Q_SIGNALS:
    void message(QList<QString> arg);
    void sendSearchResult(QStringList arg);

public Q_SLOTS:
    void slotFunc(QString arg);
    void recvSearchKeyword(QString arg);

private:
    QString m_keyWord;
    QMap<QString,QString> recMap;
};

#endif // SEARCHFILETHREAD_H
