#include "search-task-plugin-manager.h"
#include <QDebug>
using namespace UkuiSearch;
static SearchTaskPluginManager *global_instance = nullptr;
SearchTaskPluginManager *SearchTaskPluginManager::getInstance()
{
    if (!global_instance) {
        global_instance = new SearchTaskPluginManager;
    }
    return global_instance;
}

bool SearchTaskPluginManager::registerPlugin(SearchTaskPluginIface *plugin)
{
    //这里把内部插件和外部插件分开写是考虑到异常处理，因为内部插件我们可以保证质量，但外部插件可能有各种各样的问题。
    //但异常处理我没想好怎么加。。
    //by zpf
    if(plugin->getCustomSearchType().isEmpty() || m_loadedPlugin.contains(plugin->getCustomSearchType())) {
        qWarning() << "search task plugin load failed! plugin type:" << plugin->getCustomSearchType();
        return false;
    }
    m_loadedPlugin.insert(plugin->getCustomSearchType(), plugin);
    connect(plugin, &SearchTaskPluginIface::searchFinished, this, &SearchTaskPluginManager::searchFinished);
    return true;
}

bool SearchTaskPluginManager::registerBuildinPlugin(SearchTaskPluginIface *plugin)
{
    m_buildinPlugin.insert(static_cast<int>(plugin->getSearchType()), plugin);
    connect(plugin, &SearchTaskPluginIface::searchFinished, this, &SearchTaskPluginManager::searchFinished);
    return true;
}

SearchTaskPluginManager::SearchTaskPluginManager(QObject *parent) : QObject(parent)
{
    //register buildin plugin into m_buildinPlugin
    //registerBuildinPlugin();
}

void SearchTaskPluginManager::pluginSearch(SearchType searchType, SearchController searchController)
{
    int type = static_cast<int>(searchType);
    if(m_buildinPlugin.contains(type)) {
        if(!m_buildinPlugin.value(type)->isEnable()) {
            Q_EMIT this->pluginDisable(searchController.getCurrentSearchId());
            return;
        }
        m_buildinPlugin.value(type)->startSearch(searchController);
    }
}

void SearchTaskPluginManager::pluginSearch(QString customSearchType, SearchController searchController)
{
    if(m_loadedPlugin.contains(customSearchType)) {
        if(!m_loadedPlugin.value(customSearchType)->isEnable()) {
            Q_EMIT this->pluginDisable(searchController.getCurrentSearchId());
            return;
        }
        m_loadedPlugin.value(customSearchType)->startSearch(searchController);
    }
}
