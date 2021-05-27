#ifndef SEARCHPLUGINFACTORY_H
#define SEARCHPLUGINFACTORY_H

#include <QObject>
#include "search-plugin-iface.h"

namespace Zeeker {
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
    QHash<QString, SearchPluginIface*> m_hash;

    explicit SearchPluginManager(QObject *parent = nullptr);
    ~SearchPluginManager();

};
}

#endif // SEARCHPLUGINFACTORY_H
