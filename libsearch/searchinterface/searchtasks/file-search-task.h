#ifndef FILESEARCHTASK_H
#define FILESEARCHTASK_H
#include "search-task-plugin-iface.h"
#include "search-controller.h"
#include <QIcon>
namespace UkuiSearch {
class FileSearchTask : public SearchTaskPluginIface
{
    Q_OBJECT
public:
    explicit FileSearchTask(SearchController searchController);
    PluginType pluginType() {return PluginType::SearchTaskPlugin;}
    const QString name();
    const QString description();
    const QIcon icon() {return QIcon::fromTheme("folder");}
    void setEnable(bool enable) {}
    bool isEnable() {}

    SearchType getSearchType() {return SearchType::File;}
    QString getCustomSearchType();
    void startSearch(SearchController searchController);
    void stop();
protected:
    void run();
private:
    SearchController m_searchControl;
};
}
#endif // FILESEARCHTASK_H
