#ifndef SEARCHTASKPLUGINMANAGER_H
#define SEARCHTASKPLUGINMANAGER_H

#include <QObject>
#include <QUuid>
#include <QMap>
#include "search-task-plugin-iface.h"

namespace UkuiSearch {

class ManagedPlugin : public QObject
{
    Q_OBJECT
public:
    explicit ManagedPlugin(QObject *parent) : QObject(parent) {}
    ~ManagedPlugin() override;

    inline SearchTaskPluginIface *internalPlugin(const SearchType& searchType);
    inline SearchTaskPluginIface *externalPlugin(const QString& customType);

    bool insertInternalPlugin(const SearchType& searchType, SearchTaskPluginIface* plugin);
    bool insertExternalPlugin(const QString& customType, SearchTaskPluginIface* plugin);

private:
    QMap<size_t, SearchTaskPluginIface*> m_internalPlugins;
    QMap<QString, SearchTaskPluginIface*> m_externalPlugin;
};

class SearchTaskPluginManager : public QObject
{
    Q_OBJECT
public:
    static SearchTaskPluginManager *getInstance();
    void initPlugins(SearchType searchType);
    SearchTaskPluginIface *initPlugins(const QUuid&, SearchType, const QString& customType = QString());
    bool registerPlugin(SearchTaskPluginIface *plugin);
    bool registerBuildinPlugin(SearchTaskPluginIface *plugin);
    void pluginSearch(SearchType searchType, std::shared_ptr<SearchController> searchController);
    void pluginSearch(QString customSearchType, std::shared_ptr<SearchController> searchController);
    bool startSearch(const QUuid&, std::shared_ptr<SearchController>, SearchType, const QString& customType = QString());
    void destroyPlugins(const QUuid& uuid);

    SearchTaskPluginIface *getPlugin(SearchType searchType, const QString& customType = QString());
    void registerPluginPath(const QString& customType, const QString& pluginPath);

Q_SIGNALS:
    void searchFinished(size_t searchId);
    void searchError(size_t searchId, QString msg);

private:
    explicit SearchTaskPluginManager(QObject *parent = nullptr);

    //这里初衷是把内外部插件分开管理，内部插件可以增加枚举值，外部插件似乎只能用编写者自定义的字符串区分？
    QHash<size_t, SearchTaskPluginIface*> m_buildinPlugin;
    QHash<QString, SearchTaskPluginIface*> m_loadedPlugin;
    QMap<QString, QString> m_loadedPluginPath;
    QMap<QUuid, ManagedPlugin*> m_managedPlugins;
};
}

#endif // SEARCHTASKPLUGINMANAGER_H
