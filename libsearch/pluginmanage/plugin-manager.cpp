/*
 * Copyright (C) 2020, KylinSoft Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: zhangpengfei <zhangpengfei@kylinos.cn>
 * Modified by: zhangzihao <zhangzihao@kylinos.cn>
 *
 */
#include "plugin-manager.h"
#include "search-plugin-manager.h"

using namespace Zeeker;

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
        if (pluginLoader.metaData().value("MetaData").toObject().value("version").toString() != VERSION)
            continue;

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
