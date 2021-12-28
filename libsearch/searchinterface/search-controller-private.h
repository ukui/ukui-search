#ifndef SEARCHCONTROLLERPRIVATE_H
#define SEARCHCONTROLLERPRIVATE_H

#include <QMutex>
#include <QSharedPointer>
#include "search-controller.h"
namespace UkuiSearch {

class SearchControllerPrivate
{
public:
    explicit SearchControllerPrivate(SearchController *parent);
    ~SearchControllerPrivate();
    size_t refreshSearchId();
    DataQueue<ResultItem>* refreshDataqueue();
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
    DataQueue<ResultItem>* m_dataQueue = nullptr ;
    size_t m_searchId = 0;
    QMutex m_searchIdMutex;
    SearchController *q;
    QSharedPointer<SearchController> m_formerController;

    QStringList m_keywords;
    QStringList m_searchDirs;
    QStringList m_FileLabels;
    bool m_recurse = true;
    bool m_activeKeywordSegmentation = false;

};
}

#endif // SEARCHCONTROLLERPRIVATE_H
