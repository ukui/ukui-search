#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include <QDebug>
#include <QDir>
#include <QPluginLoader>
#include <QApplication>
#include "plugin-iface.h"
#include "search-plugin-iface.h"
#include "search-task-plugin-iface.h"

namespace UkuiSearch {
class LIBSEARCH_EXPORT PluginManager : public QObject
{
    Q_OBJECT
public:
    static void init();
    static PluginManager *getInstance();
    void close();

Q_SIGNALS:
    void pluginStateChanged(const QString &pluginName, bool enable);

public Q_SLOTS:
    void setPluginEnableByName(const QString &pluginName, bool enable);

private:
    explicit PluginManager(QObject *parent = nullptr);

    ~PluginManager();

    QHash<QString, PluginInterface*> m_hash;

};
}

#endif // PLUGINMANAGER_H
