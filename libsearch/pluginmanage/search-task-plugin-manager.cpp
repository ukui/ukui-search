#include "search-task-plugin-manager.h"
#include <QDebug>
#include <QPluginLoader>

#include "file-search-task.h"
#include "file-content-search-task.h"
#include "app-search-task.h"

using namespace UkuiSearch;
static SearchTaskPluginManager *global_instance = nullptr;
SearchTaskPluginManager *SearchTaskPluginManager::getInstance()
{
    if (!global_instance) {
        global_instance = new SearchTaskPluginManager;
    }
    return global_instance;
}

void SearchTaskPluginManager::initPlugins(SearchType searchType)
{
    switch (searchType) {
        case SearchType::File:
            registerBuildinPlugin(new FileSearchTask(this));
            break;
        case SearchType::FileContent:
            registerBuildinPlugin(new FileContentSearchTask(this));
            break;
        case SearchType::Application:
            registerBuildinPlugin(new AppSearchTask(this));
        default:
            break;
    }
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
    connect(plugin, &SearchTaskPluginIface::searchError, this, &SearchTaskPluginManager::searchError);
    return true;
}

bool SearchTaskPluginManager::registerBuildinPlugin(SearchTaskPluginIface *plugin)
{
    m_buildinPlugin.insert(static_cast<size_t>(plugin->getSearchType()), plugin);
    connect(plugin, &SearchTaskPluginIface::searchFinished, this, &SearchTaskPluginManager::searchFinished);
    connect(plugin, &SearchTaskPluginIface::searchError, this, &SearchTaskPluginManager::searchError);
    return true;
}

SearchTaskPluginManager::SearchTaskPluginManager(QObject *parent) : QObject(parent)
{
}

void SearchTaskPluginManager::pluginSearch(SearchType searchType, std::shared_ptr<SearchController> searchController)
{
    size_t type = static_cast<size_t>(searchType);
    qDebug() << "search type" << type;
    if(m_buildinPlugin.contains(type)) {
        if(!m_buildinPlugin.value(type)->isEnable()) {
            Q_EMIT this->searchError(searchController.get()->getCurrentSearchId(), tr("plugin type: %1, is disabled!").arg(type));
            return;
        }
        qDebug() << "start search";
        m_buildinPlugin.value(type)->startSearch(searchController);
    } else {
        Q_EMIT this->searchError(searchController.get()->getCurrentSearchId(), tr("plugin type: %1, is not registered!").arg(type));
    }
}

void SearchTaskPluginManager::pluginSearch(QString customSearchType, std::shared_ptr<SearchController> searchController)
{
    if(m_loadedPlugin.contains(customSearchType)) {
        if(!m_loadedPlugin.value(customSearchType)->isEnable()) {
            Q_EMIT this->searchError(searchController.get()->getCurrentSearchId(), tr("plugin type: %1, is disabled!").arg(customSearchType));
            return;
        }
        m_loadedPlugin.value(customSearchType)->startSearch(searchController);
    } else {
        Q_EMIT this->searchError(searchController.get()->getCurrentSearchId(), tr("plugin type: %1, is not registered!").arg(customSearchType));
    }
}

bool SearchTaskPluginManager::startSearch(const QUuid &uuid, std::shared_ptr<SearchController> searchController, SearchType searchType, const QString &customType)
{
    if (m_managedPlugins.contains(uuid)) {
        ManagedPlugin* managedPlugin = m_managedPlugins.value(uuid);

        SearchTaskPluginIface *plugin = nullptr;
        if (searchType == SearchType::Custom) {
            plugin = managedPlugin->externalPlugin(customType);

        } else {
            plugin = managedPlugin->internalPlugin(searchType);
        }

        if (plugin) {
            plugin->startSearch(std::move(searchController));
            return true;
        }
    }

    return false;
}

SearchTaskPluginIface *SearchTaskPluginManager::getPlugin(SearchType searchType, const QString& customType)
{
    SearchTaskPluginIface *searchPlugin = nullptr;
    if (searchType == SearchType::Custom) {
        if (m_loadedPluginPath.contains(customType)) {
            QPluginLoader pluginLoader(m_loadedPluginPath.value(customType));
            QObject *plugin = pluginLoader.instance();
            if (plugin) {
                searchPlugin = dynamic_cast<SearchTaskPluginIface*>(plugin);
            }
        }

    } else {
        switch (searchType) {
            case SearchType::File:
                searchPlugin = new FileSearchTask(this);
                break;
            case SearchType::FileContent:
                searchPlugin = new FileContentSearchTask(this);
                break;
            case SearchType::Application:
                searchPlugin = new AppSearchTask(this);
                break;
            default:
                break;
        }
    }

    return searchPlugin;
}

void SearchTaskPluginManager::registerPluginPath(const QString& customType, const QString &pluginPath)
{
    if (!m_loadedPluginPath.contains(customType)) {
        m_loadedPluginPath.insert(customType, pluginPath);
    }
}

SearchTaskPluginIface *SearchTaskPluginManager::initPlugins(const QUuid& uuid, SearchType searchType, const QString& customType)
{
    if (!m_managedPlugins.contains(uuid)) {
        m_managedPlugins.insert(uuid, new ManagedPlugin(nullptr));
    }

    SearchTaskPluginIface *plugin = getPlugin(searchType, customType);
    bool succeed = false;
    if (searchType == SearchType::Custom) {
        succeed = m_managedPlugins.value(uuid)->insertExternalPlugin(customType, plugin);

    } else {
        succeed = m_managedPlugins.value(uuid)->insertInternalPlugin(searchType, plugin);
    }

    return succeed ? plugin : nullptr;
}

void SearchTaskPluginManager::destroyPlugins(const QUuid &uuid)
{
    if (m_managedPlugins.contains(uuid)) {
        ManagedPlugin *managedPlugin = m_managedPlugins.value(uuid);
        m_managedPlugins.remove(uuid);

        delete managedPlugin;
    }
}

ManagedPlugin::~ManagedPlugin()
{

}

bool ManagedPlugin::insertInternalPlugin(const SearchType &searchType, SearchTaskPluginIface *plugin)
{
    if (plugin) {
        auto type = static_cast<size_t>(searchType);
        plugin->setParent(this);

        if (!m_internalPlugins.contains(type)) {
            m_internalPlugins.insert(type, plugin);
            return true;
        }

        plugin->deleteLater();
    }

    return false;
}

bool ManagedPlugin::insertExternalPlugin(const QString &customType, SearchTaskPluginIface *plugin)
{
    if (plugin) {
        plugin->setParent(this);
        if (!customType.isEmpty() && !m_externalPlugin.contains(customType)) {
            m_externalPlugin.insert(customType, plugin);
            return true;
        }

        plugin->deleteLater();
    }

    return false;
}

inline SearchTaskPluginIface *ManagedPlugin::internalPlugin(const SearchType &searchType)
{
    auto type = static_cast<size_t>(searchType);
    if (m_internalPlugins.contains(type)) {
        return m_internalPlugins.value(type);
    }
    return nullptr;
}

inline SearchTaskPluginIface *ManagedPlugin::externalPlugin(const QString &customType)
{
    if (m_externalPlugin.contains(customType)) {
        return m_externalPlugin.value(customType);
    }
    return nullptr;
}
