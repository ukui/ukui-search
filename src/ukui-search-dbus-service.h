/*
 * Copyright (C) 2022, KylinSoft Co., Ltd.
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
 *
 */
#ifndef UKUISEARCHDBUSSERVICE_H
#define UKUISEARCHDBUSSERVICE_H

#include <QDBusConnection>
#include <QObject>

#include "mainwindow.h"

namespace Zeeker {

class UkuiSearchDbusServices: public QObject{
    Q_OBJECT

    Q_CLASSINFO("D-Bus Interface","org.ukui.search.service")

public:
    explicit UkuiSearchDbusServices(MainWindow *m);
    ~UkuiSearchDbusServices();

public Q_SLOTS:
    void showWindow();

private:
    MainWindow *m_mainWindow;
};
}

#endif // UKUISEARCHDBUSSERVICE_H
