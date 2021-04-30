#ifndef SEARCHMETHODMANAGER_H
#define SEARCHMETHODMANAGER_H

#include "first-index.h"
#include "inotify-index.h"

namespace Zeeker {
class SearchMethodManager {
public:
    SearchMethodManager() = default;
    void searchMethod(FileUtils::SearchMethod sm);
private:
    FirstIndex m_fi;
    InotifyIndex* m_ii;
};
}

#endif // SEARCHMETHODMANAGER_H
