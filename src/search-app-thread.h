#ifndef SEARCHAPPTHREAD_H
#define SEARCHAPPTHREAD_H
#include <QThread>
#include "libsearch.h"

class SearchAppThread : public QThread
{
    Q_OBJECT

public:
    SearchAppThread(QObject * parent = nullptr);
    ~SearchAppThread();
    void startSearch(const QString&);
    void stop();
protected:
    void run() override;
private:
    AppMatch * m_appMatch = nullptr;
    QString m_keyword;
    bool m_stop = false;
Q_SIGNALS:
    void searchResultApp(const QVector<QStringList>&);
};

#endif // SEARCHAPPTHREAD_H
