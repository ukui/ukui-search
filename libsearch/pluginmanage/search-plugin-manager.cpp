#include <QDebug>
#include "search-plugin-manager.h"
#include "file-search-plugin.h"
#include "app-search-plugin.h"-
#include "settings-search-plugin.h"

using namespace Zeeker;

static SearchPluginManager *global_instance = nullptr;
SearchPluginManager::SearchPluginManager(QObject *parent)
{
    registerPlugin(new AppSearchPlugin(this));
    registerPlugin(new SettingsSearchPlugin(this));
    registerPlugin(new DirSearchPlugin(this));
    registerPlugin(new FileSearchPlugin(this));
    registerPlugin(new FileContengSearchPlugin(this));

}

bool SearchPluginManager::registerPlugin(Zeeker::SearchPluginIface *plugin)
{
    if (m_map.end() != m_map.find(plugin->name())){
        return false;
    }
    m_map[plugin->name()] = plugin;
    return true;
}

SearchPluginManager *SearchPluginManager::getInstance()
{
    if (!global_instance) {
        global_instance = new SearchPluginManager;
    }
    return global_instance;
}

const QStringList SearchPluginManager::getPluginIds()
{
    QStringList list;
    for (auto i = m_map.begin(); i != m_map.end(); i++) {
        list.append((*i).first);
    }
    return list;
}

SearchPluginIface *SearchPluginManager::getPlugin(const QString &pluginId)
{
    return m_map[pluginId];
}

void SearchPluginManager::close()
{
    this->deleteLater();
}

SearchPluginManager::~SearchPluginManager()
{
    m_map.clear();
}
