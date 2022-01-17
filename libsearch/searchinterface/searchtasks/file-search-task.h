#ifndef FILESEARCHTASK_H
#define FILESEARCHTASK_H

#include <QIcon>
#include <QThreadPool>
#include <QRunnable>
#include "search-task-plugin-iface.h"
#include "search-controller.h"
#include "result-item.h"
namespace UkuiSearch {
/*
 * 这里只写了大概框架，具体逻辑未实现，可以当成伪代码参考。
*/
class FileSearchTask : public SearchTaskPluginIface
{
    Q_OBJECT
public:
    explicit FileSearchTask(SearchController searchController);
    PluginType pluginType() {return PluginType::SearchTaskPlugin;}
    const QString name();
    const QString description();
    const QIcon icon() {return QIcon::fromTheme("folder");}
    void setEnable() {}
    bool isEnable() { return true;}

    SearchType getSearchType() {return SearchType::File;}
    QString getCustomSearchType();
    void startSearch(SearchController searchController);
    void stop();
    Q_INVOKABLE void sendFinishSignal(size_t searchId);
protected:
    void run();
private:
    SearchController m_searchControl;
    QThreadPool *m_pool = nullptr;
};

class FileSearchWorker : public QRunnable
{
public:
    explicit FileSearchWorker(FileSearchTask *fileSarchTask, SearchController searchController);
protected:
    void run();
private:
    FileSearchTask *m_FileSearchTask;
    SearchController m_searchController;
};
}
#endif // FILESEARCHTASK_H
