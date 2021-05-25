/*
 *
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
 * Authors: zhangjiaping <zhangjiaping@kylinos.cn>
 *
 */
#ifndef SEARCHPAGESECTION_H
#define SEARCHPAGESECTION_H
#include <QScrollArea>
#include <QPaintEvent>
#include <QPainter>
#include <QStyleOption>
#include "result-view.h"
#include "plugininterface/search-plugin-iface.h"

namespace Zeeker {
class ResultArea : public QScrollArea
{
    Q_OBJECT
public:
    ResultArea(QWidget *parent = nullptr);
    ~ResultArea() = default;
    void appendWidet(ResultWidget *);
    void setVisibleList(const QStringList &);
private:
    void initUi();
    void setupConnectionsForWidget(ResultWidget *);
    QWidget * m_widget = nullptr;
    QVBoxLayout * m_mainLyt = nullptr;
    QList<ResultWidget *> m_widget_list;

Q_SIGNALS:
    void startSearch(const QString &);
    void stopSearch();

};

class DetailWidget : public QWidget
{
    Q_OBJECT
public:
    DetailWidget(QWidget *parent = nullptr);
    ~DetailWidget() = default;
    void clear();

public Q_SLOTS:
    void setWidgetInfo(const QString&, const SearchPluginIface::ResultInfo&);

private:
    void initUi();
    void paintEvent(QPaintEvent *);
    void clearLayout(QLayout *);
    QVBoxLayout * m_mainLyt = nullptr;
    QLabel * m_iconLabel = nullptr;
    QFrame * m_nameFrame = nullptr;
    QHBoxLayout * m_nameFrameLyt = nullptr;
    QLabel * m_nameLabel = nullptr;
    QLabel * m_pluginLabel = nullptr;
    QFrame * m_line_1 = nullptr;
    QFrame * m_descFrame = nullptr;
    QVBoxLayout * m_descFrameLyt = nullptr;
    QFrame * m_line_2 = nullptr;
    QFrame * m_actionFrame = nullptr;
    QVBoxLayout * m_actionFrameLyt = nullptr;
};

class DetailArea : public QScrollArea
{
    Q_OBJECT
public:
    DetailArea(QWidget *parent = nullptr);
    ~DetailArea() = default;
private:
    void initUi();
    DetailWidget * m_detailWidget = nullptr;

Q_SIGNALS:
    void setWidgetInfo(const QString&, const SearchPluginIface::ResultInfo&);
};

class ActionLabel : public QLabel
{
    Q_OBJECT
public:
    ActionLabel(const QString &action, const QString &key, const QString &plugin, QWidget *parent = nullptr);
    ~ActionLabel() = default;
private:
    void initUi();
    QString m_action;
    QString m_key;
    QString m_plugin;

protected:
    bool eventFilter(QObject *, QEvent *);
};

}

#endif // SEARCHPAGESECTION_H
