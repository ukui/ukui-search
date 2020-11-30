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


Q_SIGNALS:
    void sendSearchResult(QStringList arg);

public Q_SLOTS:
    void recvSearchKeyword(QString arg);

private:
    QString m_keyWord; //编辑栏输入
    QMap<QString,QStringList> recMap; //接收filesearch类返回的文件名字和文件路径
    QStringList returnResult; //返回给model的字符串列表，文件的绝对路径
};

#endif // SEARCHFILETHREAD_H
