#ifndef UKUISEARCHPRIVATE_H
#define UKUISEARCHPRIVATE_H

#include "ukui-search-task.h"
#include <QThreadPool>
#include <QMutex>
#include <QSharedPointer>

#include "data-queue.h"
#include "search-controller.h"
namespace UkuiSearch {
class UkuiSearchTaskPrivate : public QObject
{
    Q_OBJECT
public:
    explicit UkuiSearchTaskPrivate(UkuiSearchTask* parent);
    ~UkuiSearchTaskPrivate();
    DataQueue<ResultItem>* init();
    void addSearchDir(QString &path);
    void setRecurse(bool recurse = true);
    void addKeyword(QString &keyword);
    void addFileLabel(QString &label);
    void setSearchOnlineApps(bool searchOnlineApps);
    size_t startSearch(SearchType searchtype);
    void stop();

private:
    SearchController* m_searchCotroller;
    size_t m_searchId = 0;
    UkuiSearchTask* q = nullptr;

Q_SIGNALS:
    void searchFinished(size_t searchId);
};
}

#endif // UKUISEARCHPRIVATE_H
