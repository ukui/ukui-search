#ifndef FILESEARCHTASK_H
#define FILESEARCHTASK_H

#include <QIcon>
#include <QThreadPool>
#include <QRunnable>
#include <xapian.h>

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
    explicit FileSearchTask(QObject *parent);
    PluginType pluginType() {return PluginType::SearchTaskPlugin;}
    const QString name();
    const QString description();
    const QIcon icon() {return QIcon::fromTheme("folder");}
    void setEnable(bool enable) {}
    bool isEnable() { return true;}

    SearchType getSearchType() {return SearchType::File;}
    QString getCustomSearchType();
    void startSearch(std::shared_ptr<SearchController> searchController);
    void stop();
    Q_INVOKABLE void sendFinishSignal(size_t searchId);

private:
    QThreadPool *m_pool = nullptr;
};

class FileSearchWorker : public QRunnable
{
    friend class FileSearchFilter;

public:
    explicit FileSearchWorker(FileSearchTask *fileSarchTask, std::shared_ptr<SearchController> searchController);

protected:
    void run();

private:
    /**
     * @brief 通过索引进行搜索，如果搜索过程正常，返回true
     * 如果搜索被打断，返回false.
     * 搜索被打断是指用户使用同一个task发起多次搜索，导致searchId发生变化，那么上一次搜索即被打断。
     * @return
     */
    bool searchWithIndex();
    //同上
    bool directSearch();
    Xapian::Query creatQueryForFileSearch();
    void sendErrorMsg(const QString &msg);

private:
    FileSearchTask *m_FileSearchTask;
    std::shared_ptr<SearchController> m_searchController;

    size_t m_currentSearchId = 0;
    QStringList m_validDirectories;
    QStringList m_blackList;
};

class FileSearchFilter : public Xapian::MatchDecider {
public:
    explicit FileSearchFilter(FileSearchWorker *parent);
    bool operator ()(const Xapian::Document &doc) const;

private:
    FileSearchWorker *parent = nullptr;
};

}
#endif // FILESEARCHTASK_H
