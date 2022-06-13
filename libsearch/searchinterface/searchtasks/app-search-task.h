#ifndef APPSEARCHTASK_H
#define APPSEARCHTASK_H

#include <QIcon>
#include <QThreadPool>
#include <QRunnable>
#include <QDBusInterface>
#include <QtDBus>
#include "search-task-plugin-iface.h"
#include "search-controller.h"
#include "result-item.h"
#include "app-info-table.h"

namespace UkuiSearch {
class AppSearchTask : public SearchTaskPluginIface
{
    Q_OBJECT
public:
    explicit AppSearchTask(QObject *parent);
    PluginType pluginType() {return PluginType::SearchTaskPlugin;}
    const QString name();
    const QString description();
    const QIcon icon() {return QIcon::fromTheme("appsearch");}
    void setEnable(bool enable) {}
    bool isEnable() { return true;}

    SearchType getSearchType() {return SearchType::Application;}
    QString getCustomSearchType();
    void startSearch(std::shared_ptr<SearchController> searchController);
    void stop();
    Q_INVOKABLE void sendFinishSignal(size_t searchId);

private:
    QThreadPool *m_pool = nullptr;
};

class AppSearchWorker : public QRunnable
{
public:
    explicit AppSearchWorker(AppSearchTask *AppSarchTask, std::shared_ptr<SearchController> searchController);

protected:
    void run();

private:
    ~AppSearchWorker();
    void sendErrorMsg(const QString &msg);

private:
    AppInfoTable m_appInfoTable;
    AppSearchTask *m_AppSearchTask = nullptr;
    std::shared_ptr<SearchController> m_searchController;
    QDBusInterface *m_interFace = nullptr;
    size_t m_currentSearchId = 0;
};
}
#endif // APPSEARCHTASK_H
