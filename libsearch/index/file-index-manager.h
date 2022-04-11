#ifndef SEARCHMETHODMANAGER_H
#define SEARCHMETHODMANAGER_H

#include <QObject>
#include <QSystemSemaphore>
#include "first-index.h"
//#include "inotify-index.h"
#include "inotify-watch.h"
namespace UkuiSearch {
class FileIndexManager : public QObject {
    Q_OBJECT
public:
    static FileIndexManager *getInstance();
    void searchMethod(FileUtils::SearchMethod sm);
private:
    FileIndexManager(QObject *parent = nullptr);
    FirstIndex *m_fi;
//    InotifyIndex* m_ii;
    InotifyWatch *m_iw = nullptr;
    QSystemSemaphore m_semaphore;
};
}

#endif // SEARCHMETHODMANAGER_H
