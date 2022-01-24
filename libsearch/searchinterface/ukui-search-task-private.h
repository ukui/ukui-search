#ifndef UKUISEARCHPRIVATE_H
#define UKUISEARCHPRIVATE_H

#include "ukui-search-task.h"
#include <QThreadPool>
#include <QMutex>
#include <memory>

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
    void setOnlySearchFile(bool onlySearchFile);
    void setOnlySearchDir(bool onlySearchDir);
    void setSearchOnlineApps(bool searchOnlineApps);
    void initSearchPlugin(SearchType searchType);
    size_t startSearch(SearchType searchtype, QString customSearchType = QString());
    void stop();

private:
    std::shared_ptr<SearchController> m_searchCotroller = nullptr;
    size_t m_searchId = 0;
    UkuiSearchTask* q = nullptr;

Q_SIGNALS:
    void searchFinished(size_t searchId);
};
}

#endif // UKUISEARCHPRIVATE_H
