#ifndef SEARCHCONTROLLER_H
#define SEARCHCONTROLLER_H

#include <QStringList>
#include <memory>
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
    explicit SearchController(std::shared_ptr<SearchController> parent = nullptr);
    ~SearchController();
    DataQueue<ResultItem>* refreshDataqueue();
    size_t refreshSearchId();
    DataQueue<ResultItem>* initDataQueue();
    void stop();

    void addSearchDir(QString &path);
    void setRecurse(bool recurse = true);
    void addKeyword(QString &keyword);
    void setActiveKeywordSegmentation(bool active);
    void addFileLabel(QString &label);
    void setOnlySearchFile(bool onlySearchFile);
    void setOnlySearchDir(bool onlySearchDir);
    //以上方法插件请不要调用

    //以下方法插件可以调用
    size_t getCurrentSearchId();
    DataQueue<ResultItem>* getDataQueue();
    bool beginSearchIdCheck(size_t searchId);
    void finishSearchIdCheck();

    QStringList getSearchDir();
    bool isRecurse();
    QStringList getKeyword();
    bool isKeywordSegmentationActived();
    QStringList getFileLabel();
    bool isSearchFileOnly();
    bool isSearchDirOnly();
private:
    std::shared_ptr<SearchController> m_parent = nullptr;
    SearchControllerPrivate *d = nullptr;

};
}

#endif // SEARCHCONTROLLER_H
