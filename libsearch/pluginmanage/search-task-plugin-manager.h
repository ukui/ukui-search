#ifndef SEARCHTASKPLUGINMANAGER_H
#define SEARCHTASKPLUGINMANAGER_H

#include <QObject>
#include "search-task-plugin-iface.h"
namespace UkuiSearch {

class SearchTaskPluginManager : public QObject
{
    Q_OBJECT
public:
    static SearchTaskPluginManager *getInstance();
    bool registerPlugin(SearchTaskPluginIface *plugin);
    bool registerBuildinPlugin(SearchTaskPluginIface *plugin);
    void pluginSearch(SearchType searchType, SearchController searchController);
    void pluginSearch(QString customSearchType, SearchController searchController);
Q_SIGNALS:
    void searchFinished(size_t searchId);
    void pluginDisable(size_t searchId);
private:
    explicit SearchTaskPluginManager(QObject *parent = nullptr);

    //这里初衷是把内外部插件分开管理，内部插件可以增加枚举值，外部插件似乎只能用编写者自定义的字符串区分？
    QHash<int, SearchTaskPluginIface*> m_buildinPlugin;
    QHash<QString, SearchTaskPluginIface*> m_loadedPlugin;

};
}

#endif // SEARCHTASKPLUGINMANAGER_H
