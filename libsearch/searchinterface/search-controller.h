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
/*
 *搜索控制，用于传递搜索条件，搜索唯一ID，以及管理队列等。
 *为树形结构，所有子节点含有智能指针指向父节点。
 *只要有插件还在使用资源，其对应的子节点以及通向根节点上所有的实例就不会被释放，析构时，从下往上由智能指针自动析构
 */
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
