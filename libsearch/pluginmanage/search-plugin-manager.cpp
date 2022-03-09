#include <QDebug>
#include "search-plugin-manager.h"
#include "file-search-plugin.h"
#include "app-search-plugin.h"
#include "settings-search-plugin.h"
#include "note-search-plugin.h"
#include "web-search-plugin.h"
#include "mail-search-plugin.h"
#include "ocr-search-plugin.h"

using namespace Zeeker;

static SearchPluginManager *global_instance = nullptr;
SearchPluginManager::SearchPluginManager(QObject *parent)
{
    registerPlugin(new AppSearchPlugin(this));
    registerPlugin(new NoteSearchPlugin(this));
    registerPlugin(new SettingsSearchPlugin(this));
    registerPlugin(new DirSearchPlugin(this));
    registerPlugin(new FileSearchPlugin(this));
    registerPlugin(new FileContengSearchPlugin(this));
//    registerPlugin(new OcrSearchPlugin(this));
//    registerPlugin(new MailSearchPlugin(this));
    registerPlugin(new WebSearchPlugin(this));
}

bool SearchPluginManager::registerPlugin(Zeeker::SearchPluginIface *plugin)
{
    if (m_map.end() != m_map.find(plugin->name())){
        return false;
    }
    m_map[plugin->name()] = plugin;
    m_plugin_order[m_plugin_order.size()] = plugin->name();//按注册顺序绑定优先级
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
    for (auto i = m_plugin_order.begin(); i != m_plugin_order.end(); i++) {
        list.append(m_plugin_order[(*i).first]);//根据优先级返回plugin ID
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
