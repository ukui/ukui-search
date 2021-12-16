/*
 *
 * Copyright (C) 2021, KylinSoft Co., Ltd.
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
 * Authors: jixiaoxu <jixiaoxu@kylinos.cn>
 *
 */
#include "search-result-page.h"
//QT_BEGIN_NAMESPACE
//extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);
//QT_END_NAMESPACE
using namespace Zeeker;

#define RESULT_WIDTH 266
#define DETAIL_WIDTH 374

SearchResultPage::SearchResultPage(QWidget *parent) : QWidget(parent)
{
    initUi();
    initConnections();
    setInternalPlugins();
}

void SearchResultPage::setSize(const int&width, const int&height)
{
//    m_splitter->setFixedSize(width, height);
}

void SearchResultPage::setInternalPlugins()
{
    Q_FOREACH (QString plugin_id, SearchPluginManager::getInstance()->getPluginIds()) {
        ResultWidget * widget = new ResultWidget(plugin_id, m_resultArea);
        m_resultArea->appendWidet(widget);
        setupConnectionsForWidget(widget);
    }
}

void SearchResultPage::appendPlugin(const QString &plugin_id)
{
    ResultWidget * widget = new ResultWidget(plugin_id, m_resultArea);
    m_resultArea->appendWidet(widget);
    setupConnectionsForWidget(widget);
}

void SearchResultPage::pressEnter()
{
    this->m_resultArea->pressEnter();
}

void SearchResultPage::pressUp()
{
    this->m_resultArea->pressUp();
}

void SearchResultPage::pressDown()
{
    this->m_resultArea->pressDown();
}

bool SearchResultPage::getSelectedState()
{
    return m_resultArea->getSelectedState();
}

void SearchResultPage::sendResizeWidthSignal(int size)
{
    Q_EMIT this->resizeWidth(size);
}

void SearchResultPage::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(palette().base());
//    p.setOpacity(GlobalSettings::getInstance()->getValue(TRANSPARENCY_KEY).toDouble());
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(this->rect(), 16, 16);
}

void SearchResultPage::initUi()
{
    this->setFixedSize(720,688);
    m_hlayout = new QHBoxLayout(this);
    m_hlayout->setContentsMargins(16 ,18, 0, 18);  //右滚动条不需要边距；

//    m_splitter = new QSplitter(this);
//    m_splitter->setContentsMargins(0, 0, 0, 0);
//    m_splitter->setFixedSize(664, 516);
//    m_splitter->move(this->rect().topLeft().x() + 18, this->rect().topLeft().y() + 18);
    m_resultArea = new ResultArea(this);
    m_detailArea = new DetailArea(this);

    m_hlayout->addWidget(m_resultArea);
    m_hlayout->addWidget(m_detailArea);
    m_hlayout->setSpacing(0);
    this->setLayout(m_hlayout);
//    m_splitter->addWidget(m_resultArea);
//    m_splitter->addWidget(m_detailArea);
//    m_splitter->setOpaqueResize(false);
//    QList<int> size_list;
//    size_list<<664<<0;
//    m_splitter->setSizes(size_list);
//    m_splitter->handle(1)->setVisible(false); //暂时禁止拖动分隔条
}

void SearchResultPage::initConnections()
{
    connect(this, &SearchResultPage::startSearch, m_resultArea, &ResultArea::startSearch);
    connect(this, &SearchResultPage::stopSearch, m_resultArea, &ResultArea::stopSearch);
    connect(this, &SearchResultPage::startSearch, m_detailArea, &DetailArea::hide);
    connect(this, &SearchResultPage::stopSearch, m_detailArea, &DetailArea::hide);
    connect(this, &SearchResultPage::startSearch, this, [=] () {
        //页面长720 - 左边距16
        sendResizeWidthSignal(704);
    });
    connect(m_resultArea, &ResultArea::keyPressChanged, m_detailArea, &DetailArea::setWidgetInfo);
    connect(m_resultArea, &ResultArea::keyPressChanged, this, [=] () {
        //结果区域长度298 - 左边距16，下同
        sendResizeWidthSignal(282);
    });
    connect(m_resultArea, &ResultArea::getResult, this, &SearchResultPage::getResult);
    connect(this, &SearchResultPage::resizeWidth, m_resultArea, &ResultArea::resizeWidth);
    connect(m_resultArea, &ResultArea::rowClicked, this, [=] () {
        sendResizeWidthSignal(282);
    });
}

void SearchResultPage::setupConnectionsForWidget(ResultWidget *widget)
{
    connect(widget, &ResultWidget::currentRowChanged, m_detailArea, &DetailArea::setWidgetInfo);
    connect(widget, &ResultWidget::currentRowChanged, m_resultArea, &ResultArea::setSelectionInfo);
    connect(widget, &ResultWidget::rowClicked, this, [=] () {
        sendResizeWidthSignal(282);
    });
    connect(this, &SearchResultPage::resizeWidth, widget, &ResultWidget::resizeWidth);
}
