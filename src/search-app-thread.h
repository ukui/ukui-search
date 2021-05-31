#ifndef SEARCHAPPTHREAD_H
#define SEARCHAPPTHREAD_H
#include <QThread>
#include <QObject>
#include <QRunnable>
#include "libsearch.h"

namespace Zeeker {
class SearchAppThread : public QObject {
    Q_OBJECT

public:
    SearchAppThread(QObject * parent = nullptr);
    ~SearchAppThread() = default;
    void startSearch(const QString&);
    Q_INVOKABLE void sendResult(const QVector<QStringList> result);
private:
    QThreadPool m_pool;
Q_SIGNALS:
    void searchResultApp(const QVector<QStringList>);
};


class SearchApp : public QObject, public QRunnable {
    Q_OBJECT
public:
    SearchApp(const QString& keyword, size_t uniqueSymbol, QObject * parent = nullptr);
    ~SearchApp();
//    void setKeyword(const QString&);
protected:
    void run() override;
private:
    SearchAppThread *m_searchappThread = nullptr;
    QString m_keyword;
    size_t m_uniqueSymbol;
    QMap<NameString, QStringList> m_installed_apps;
    QMap<NameString, QStringList> m_uninstalled_apps;
//Q_SIGNALS:
//    void searchResultApp(const QVector<QStringList>&);
};
}

#endif // SEARCHAPPTHREAD_H
