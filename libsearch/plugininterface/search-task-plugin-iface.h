#ifndef SEARCHTASKPLUGINIFACE_H
#define SEARCHTASKPLUGINIFACE_H
#define SearchTaskPluginIface_iid "org.ukui.ukui-search.plugin-iface.SearchTaskPluginIface"

#include <QObject>
#include <QtPlugin>
#include "plugin-iface.h"
#include "common-defines.h"
#include "search-controller.h"


namespace UkuiSearch {
class SearchTaskPluginIface : public QObject, public PluginInterface
{
    Q_OBJECT
public:
    virtual QString getCustomSearchType() = 0;
    virtual SearchType getSearchType() = 0;
    //Asynchronous,multithread.
    virtual void startSearch(std::shared_ptr<SearchController> searchController) = 0;
    virtual void stop() = 0;
Q_SIGNALS:
    void searchFinished(size_t searchId);
};
}
Q_DECLARE_INTERFACE(UkuiSearch::SearchTaskPluginIface, SearchTaskPluginIface_iid)



#endif // SEARCHTASKPLUGINIFACE_H
