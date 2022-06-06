#ifndef SEARCHMETHODMANAGER_H
#define SEARCHMETHODMANAGER_H

#include <QSystemSemaphore>
#include "first-index.h"
//#include "inotify-index.h"
#include "inotify-watch.h"
namespace UkuiSearch {
class SearchMethodManager {
public:
    static SearchMethodManager *getInstance();
    void searchMethod(FileUtils::SearchMethod sm);
private:
    SearchMethodManager();
    FirstIndex *m_fi;
//    InotifyIndex* m_ii;
    InotifyWatch *m_iw = nullptr;
    QSystemSemaphore m_semaphore;
};
}

#endif // SEARCHMETHODMANAGER_H