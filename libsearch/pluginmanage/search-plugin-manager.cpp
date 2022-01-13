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
#include "search-plugin-manager.h"

using namespace Zeeker;

static SearchPluginManager *global_instance = nullptr;
bool SearchPluginManager::registerPlugin(Zeeker::SearchPluginIface *plugin)
{
    if (m_hash.value(plugin->name())) {
        return false;
    }
    m_hash.insert(plugin->name(), plugin);
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
    return m_hash.keys();
}

SearchPluginIface *SearchPluginManager::getPlugin(const QString &pluginId)
{
    return m_hash.value(pluginId);
}

void SearchPluginManager::close()
{
    this->deleteLater();
}

SearchPluginManager::SearchPluginManager(QObject *parent)
{
}

SearchPluginManager::~SearchPluginManager()
{
    m_hash.clear();
}
