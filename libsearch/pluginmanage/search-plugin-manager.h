#ifndef SEARCHPLUGINFACTORY_H
#define SEARCHPLUGINFACTORY_H

#include <QObject>
#include "search-plugin-iface.h"

namespace UkuiSearch {

class SearchPluginManager : public QObject
{
    Q_OBJECT
public:
    bool registerPlugin(SearchPluginIface *plugin);

    static SearchPluginManager *getInstance();
    const QStringList getPluginIds();
    SearchPluginIface *getPlugin(const QString &pluginId);

    void close();

private:
    std::map<QString, SearchPluginIface*> m_map;
    std::map<int, QString> m_plugin_order;//绑定plugin ID和优先级
    explicit SearchPluginManager(QObject *parent = nullptr);
    ~SearchPluginManager();

};
}

#endif // SEARCHPLUGINFACTORY_H
