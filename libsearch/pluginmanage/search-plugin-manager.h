#ifndef SEARCHPLUGINFACTORY_H
#define SEARCHPLUGINFACTORY_H

#include <QObject>
#include "search-plugin-iface.h"

namespace Zeeker {

struct cmpPluginId
{
    bool operator ()(const QString& k1, const QString& k2){
        return k1 > k2;
    }
};

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
    std::map<QString, SearchPluginIface*, cmpPluginId> m_map;
    explicit SearchPluginManager(QObject *parent = nullptr);
    ~SearchPluginManager();

};
}

#endif // SEARCHPLUGINFACTORY_H
