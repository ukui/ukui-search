#include "plugin-manager.h"
#include "search-plugin-manager.h"
#include "search-task-plugin-manager.h"

using namespace UkuiSearch;

static PluginManager *global_instance = nullptr;
void PluginManager::init()
{
    PluginManager::getInstance();
}

PluginManager *PluginManager::getInstance()
{
    if (!global_instance) {
        global_instance = new PluginManager;
    }
    return global_instance;
}

void PluginManager::setPluginEnableByName(const QString &pluginName, bool enable)
{
    m_hash.value(pluginName)->setEnable(enable);
}

PluginManager::PluginManager(QObject *parent) : QObject(parent)
{
    QDir pluginsDir(PLUGIN_INSTALL_DIRS);
    pluginsDir.setFilter(QDir::Files);

    Q_FOREACH(QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        // version check
        QString type = pluginLoader.metaData().value("MetaData").toObject().value("type").toString();
        QString version = pluginLoader.metaData().value("MetaData").toObject().value("version").toString();
        if(type == "SEARCH_PLUGIN") {
            if (version != SEARCH_PLUGIN_IFACE_VERSION) {
                qWarning() << "SEARCH_PLUGIN version check failed:" << fileName << "version:" << version << "iface version : " << SEARCH_PLUGIN_IFACE_VERSION;
                continue;
            }
        } else if(type == "SEARCH_TASK_PLUGIN") {
            if (version != SEARCH_TASK_PLUGIN_IFACE_VERSION) {
                qWarning() << "SEARCH_TASK_PLUGIN_IFACE_VERSION version check failed:" << fileName << "version:" << version << "iface version : " << SEARCH_TASK_PLUGIN_IFACE_VERSION;
                continue;
            }
        } else {
            qWarning() << "Unsupport plugin:" << fileName << "type:" << type;
            continue;
        }

        QObject *plugin = pluginLoader.instance();
        if (!plugin)
            continue;
        PluginInterface *piface = dynamic_cast<PluginInterface*>(plugin);
        if (!piface)
            continue;
        m_hash.insert(piface->name(), piface);
        switch (piface->pluginType()) {
        case PluginInterface::PluginType::SearchPlugin: {
            auto p = dynamic_cast<SearchPluginIface *>(plugin);
            SearchPluginManager::getInstance()->registerPlugin(p);
            break;
        }
        case PluginInterface::PluginType::SearchTaskPlugin: {
            auto p = dynamic_cast<SearchTaskPluginIface*>(plugin);
            SearchTaskPluginManager::getInstance()->registerPlugin(p);
            SearchTaskPluginManager::getInstance()->registerPluginPath(p->getCustomSearchType(), pluginsDir.absoluteFilePath(fileName));
            break;
        }
        default:
            break;
        }
    }
}

PluginManager::~PluginManager()
{
    m_hash.clear();
    SearchPluginManager::getInstance()->close();
}
