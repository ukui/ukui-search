#ifndef SEARCHTASKPLUGINIFACE_H
#define SEARCHTASKPLUGINIFACE_H
#define SearchTaskPLuginIface_iid "org.ukui.ukui-search.plugin-iface.SearchTaskPLuginIface"

#include <QObject>
#include <QtPlugin>
#include "plugin-iface.h"
#include "search-controller.h"
#include "common-defines.h"

namespace UkuiSearch {
class SearchTaskPLuginIface : public QObject, public PluginInterface
{
    Q_OBJECT
public:
    virtual QString getSearchType() = 0;
    //Asynchronous,multithread.
    virtual void startSearch(SearchController searchController) = 0;
    virtual void stop() = 0;
Q_SIGNALS:
    void searchFinished(size_t searchId);
};
}
Q_DECLARE_INTERFACE(UkuiSearch::SearchTaskPLuginIface, SearchTaskPLuginIface_iid)



#endif // SEARCHTASKPLUGINIFACE_H
