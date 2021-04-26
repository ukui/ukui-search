#ifndef SEARCHAPPTHREAD_H
#define SEARCHAPPTHREAD_H
#include <QThread>
#include <QObject>
#include <QRunnable>
#include "libsearch.h"

class SearchAppThread : public QObject {
    Q_OBJECT

public:
    SearchAppThread(QObject * parent = nullptr);
    ~SearchAppThread() = default;
    void startSearch(const QString&);
private:
    QThreadPool m_pool;
Q_SIGNALS:
    void searchResultApp(const QVector<QStringList>&);
};


class SearchApp : public QObject, public QRunnable {
    Q_OBJECT
public:
    SearchApp(const QString& keyword, QObject * parent = nullptr);
    ~SearchApp();
//    void setKeyword(const QString&);
protected:
    void run() override;
private:
    QString m_keyword;
    QMap<NameString, QStringList> m_installed_apps;
    QMap<NameString, QStringList> m_uninstalled_apps;
Q_SIGNALS:
    void searchResultApp(const QVector<QStringList>&);
};

#endif // SEARCHAPPTHREAD_H
