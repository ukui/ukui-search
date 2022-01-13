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
#ifndef SEARCHPLUGINFACTORY_H
#define SEARCHPLUGINFACTORY_H

#include <QObject>
#include "plugininterface/search-plugin-iface.h"

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
