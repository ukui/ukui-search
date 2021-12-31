#ifndef SEARCHCONTROLLER_H
#define SEARCHCONTROLLER_H

#include <QStringList>
#include "data-queue.h"
//todo: url parser?
namespace UkuiSearch {
class UkuiSearchTask;
class ResultItem;
class SearchControllerPrivate;
class SearchController
{
    friend class SearchControllerPrivate;
public:
    explicit SearchController(SearchController *parent = nullptr);
    ~SearchController();
    DataQueue<ResultItem>* refreshDataqueue();
    size_t refreshSearchId();
    DataQueue<ResultItem>* initDataQueue();

    void addSearchDir(QString &path);
    void setRecurse(bool recurse = true);
    void addKeyword(QString &keyword);
    void setActiveKeywordSegmentation(bool active);
    void addFileLabel(QString &label);

    size_t getCurrentSearchId();
    DataQueue<ResultItem>* getDataQueue();
    bool beginSearchIdCheck(size_t searchId);
    void finishSearchIdCheck();
    void stop();
    QStringList getSearchDir();
    bool isRecurse();
    QStringList getKeyword();
    bool isKeywordSegmentationActived();
    QStringList getFileLabel();
private:
    SearchControllerPrivate *d;
    SearchController *m_parent;
};
}

#endif // SEARCHCONTROLLER_H
