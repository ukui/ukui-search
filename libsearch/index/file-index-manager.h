#ifndef SEARCHMETHODMANAGER_H
#define SEARCHMETHODMANAGER_H

#include <QObject>
#include <QSystemSemaphore>
#include <QGSettings/QGSettings>
#include "first-index.h"
//#include "inotify-index.h"
#include "inotify-watch.h"
namespace UkuiSearch {
class FileIndexManager : public QObject {
    Q_OBJECT
public:
    static FileIndexManager *getInstance();
    void searchMethod(FileUtils::SearchMethod sm);
    void initIndexPathSetFunction();
private Q_SLOTS:
    void handleIndexPathAppend(const QString path, const QStringList blockList);
private:
    FileIndexManager(QObject *parent = nullptr);
    FirstIndex *m_fi;
//    InotifyIndex* m_ii;
    InotifyWatch *m_iw = nullptr;
    QSystemSemaphore m_semaphore;
    QGSettings *m_searchSettings = nullptr;
};
}

#endif // SEARCHMETHODMANAGER_H
