//
// Created by hxf on 2022/4/18.
//

#ifndef UKUI_SEARCH_FILE_CONTENT_SEARCH_TASK_H
#define UKUI_SEARCH_FILE_CONTENT_SEARCH_TASK_H

//ukui-search
#include "search-task-plugin-iface.h"

//Qt
#include <QThreadPool>
#include <QRunnable>

//Xapian
#include <xapian.h>

namespace UkuiSearch {

class FileContentSearchTask : public SearchTaskPluginIface
{
public:
    explicit FileContentSearchTask(QObject *parent = nullptr);

    ~FileContentSearchTask() override;

    PluginType pluginType() override;
    const QString name() override;
    const QString description() override;
    const QIcon icon() override;
    void setEnable(bool enable) override;
    bool isEnable() override;

    QString getCustomSearchType() override;
    SearchType getSearchType() override;
    void startSearch(std::shared_ptr<SearchController> searchController) override;
    void stop() override;

private:
    QThreadPool *m_pool = nullptr;
};

class FileContentSearchWorker : public QRunnable
{
    friend class FileContentSearchFilter;

public:
    explicit FileContentSearchWorker(FileContentSearchTask *fileContentSearchTask, std::shared_ptr<SearchController> searchController);

    void run() override;

private:
    bool execSearch();
    inline Xapian::Query createQuery();

    void sendErrorMsg(const QString &msg);

private:
    FileContentSearchTask *m_fileContentSearchTask = nullptr;
    std::shared_ptr<SearchController> m_searchController;

    QStringList m_validDirectories;

    size_t m_currentSearchId = 0;
};

class FileContentSearchFilter : public Xapian::MatchDecider {
public:
    explicit FileContentSearchFilter(FileContentSearchWorker*);
    bool operator ()(const Xapian::Document &doc) const override;

private:
    FileContentSearchWorker *m_worker;
};

}

#endif //UKUI_SEARCH_FILE_CONTENT_SEARCH_TASK_H
