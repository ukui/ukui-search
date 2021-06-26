#ifndef SEARCHMETHODMANAGER_H
#define SEARCHMETHODMANAGER_H

#include "first-index.h"
//#include "inotify-index.h"
#include "inotify-watch.h"
namespace Zeeker {
class SearchMethodManager {
public:
    SearchMethodManager();
    void searchMethod(FileUtils::SearchMethod sm);
private:
    FirstIndex m_fi;
//    InotifyIndex* m_ii;
    InotifyWatch *m_iw = nullptr;
};
}

#endif // SEARCHMETHODMANAGER_H
