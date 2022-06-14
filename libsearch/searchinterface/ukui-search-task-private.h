#ifndef UKUISEARCHPRIVATE_H
#define UKUISEARCHPRIVATE_H

#include "ukui-search-task.h"
#include <QThreadPool>
#include <QMutex>
#include <memory>
#include <QUuid>

#include "data-queue.h"
#include "search-controller.h"
#include "search-task-plugin-iface.h"

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
    void initSearchPlugin(SearchType searchType, const QString& customSearchType = QString());
    bool setResultDataType(SearchType searchType, ResultDataTypes dataType);
    bool setCustomResultDataType(QString customSearchType, QStringList dataType);
    void clearAllConditions();
    void clearKeyWords();
    void clearSearchDir();
    void clearFileLabel();
    void setPagination(unsigned int first, unsigned int maxResults);

    size_t startSearch(SearchType searchtype, const QString& customSearchType = QString());
    void stop();

private:
    std::shared_ptr<SearchController> m_searchCotroller = nullptr;
    size_t m_searchId = 0;
    UkuiSearchTask* q = nullptr;
    QUuid m_uuid;

Q_SIGNALS:
    void searchFinished(size_t searchId);
    void searchError(size_t searchId, QString msg);
};
}

#endif // UKUISEARCHPRIVATE_H
