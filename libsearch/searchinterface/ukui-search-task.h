#ifndef UKUISEARCH_H
#define UKUISEARCH_H

#include "result-item.h"
#include "data-queue.h"
#include "common-defines.h"
namespace UkuiSearch {
class UkuiSearchTaskPrivate;
class UkuiSearchTask : public QObject
{
    Q_OBJECT
public:
    explicit UkuiSearchTask(QObject *parent = nullptr);
    ~UkuiSearchTask();
    DataQueue<ResultItem>* init();
    void addSearchDir(QString &path);
    void setRecurse(bool recurse = true);
    void addKeyword(QString &keyword);
    void addFileLabel(QString &label);
    void setOnlySearchFile(bool onlySearchFile);
    void setOnlySearchDir(bool onlySearchDir);
    void setSearchOnlineApps(bool searchOnlineApps);
    void initSearchPlugin(SearchType searchType);
    void clearAllConditions();
    void clearKeyWords();
    void clearSearchDir();
    void setPagination(unsigned int first, unsigned int maxResults);

    size_t startSearch(SearchType searchtype, QString customSearchType = QString());
    void stop();

Q_SIGNALS:
    void searchFinished(size_t searchId);
    void searchError(size_t searchId, QString msg);

private:
    UkuiSearchTaskPrivate* d = nullptr;
};
}

#endif // UKUISEARCH_H
