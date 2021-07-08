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
#include "search-page.h"
using namespace Zeeker;

#define RESULT_WIDTH 266
#define DETAIL_WIDTH 374

SearchPage::SearchPage(QWidget *parent) : QWidget(parent)
{
    initUi();
    initConnections();
}

void SearchPage::setSize(const int&width, const int&height)
{
    m_splitter->setFixedSize(width, height);
}

void SearchPage::setPlugins(const QStringList &plugins_id)
{
    Q_FOREACH (QString plugin_id, plugins_id) {
        ResultWidget * widget = new ResultWidget(plugin_id, m_resultArea);
        m_resultArea->appendWidet(widget);
        setupConnectionsForWidget(widget);
    }
}

void SearchPage::appendPlugin(const QString &plugin_id)
{
    ResultWidget * widget = new ResultWidget(plugin_id, m_resultArea);
    m_resultArea->appendWidet(widget);
    setupConnectionsForWidget(widget);
}

void SearchPage::initUi()
{
    m_splitter = new QSplitter(this);
    m_splitter->setContentsMargins(0, 0, 0, 0);
    m_resultArea = new ResultArea(m_splitter);
    m_detailArea = new DetailArea(m_splitter);
    m_splitter->addWidget(m_resultArea);
    m_splitter->addWidget(m_detailArea);
    m_splitter->setOpaqueResize(false);
    QList<int> size_list;
    size_list<<RESULT_WIDTH<<DETAIL_WIDTH;
    m_splitter->setSizes(size_list);
    m_splitter->handle(1)->setEnabled(false); //暂时禁止拖动分隔条
}

void SearchPage::initConnections()
{
    connect(this, &SearchPage::startSearch, m_resultArea, &ResultArea::startSearch);
    connect(this, &SearchPage::stopSearch, m_resultArea, &ResultArea::stopSearch);
}

void SearchPage::setupConnectionsForWidget(ResultWidget *widget)
{
    connect(widget, &ResultWidget::currentRowChanged, m_detailArea, &DetailArea::setWidgetInfo);
    connect(widget, &ResultWidget::currentRowChanged, this, &SearchPage::currentRowChanged);
    connect(this, &SearchPage::currentRowChanged, widget, &ResultWidget::clearSelectedRow);
    connect(widget, &ResultWidget::rowClicked, this, &SearchPage::effectiveSearch);
}
