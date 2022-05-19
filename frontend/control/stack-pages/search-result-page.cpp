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
#include <QPainterPath>
QT_BEGIN_NAMESPACE
extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);
QT_END_NAMESPACE
using namespace UkuiSearch;

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
    p.setOpacity(GlobalSettings::getInstance()->getValue(TRANSPARENCY_KEY).toDouble());
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(this->rect().adjusted(10,10,-10,-10), 12, 12);

    QPainterPath rectPath;
    rectPath.addRoundedRect(this->rect().adjusted(10, 10, -10, -10), 12, 12);

    // 画一个黑底
    QPixmap pixmap(this->rect().size());
    pixmap.fill(Qt::transparent);
    QPainter pixmapPainter(&pixmap);
    pixmapPainter.setRenderHint(QPainter::Antialiasing);
//      pixmapPainter.setCompositionMode(QPainter::CompositionMode_Difference);
    pixmapPainter.setPen(Qt::transparent);
    pixmapPainter.setBrush(Qt::black);
    pixmapPainter.setOpacity(0.65);
    pixmapPainter.drawPath(rectPath);
    pixmapPainter.end();


    // 模糊这个黑底
    QImage img = pixmap.toImage();
    qt_blurImage(img, 10, false, false);


    // 挖掉中心
    pixmap = QPixmap::fromImage(img);
    QPainter pixmapPainter2(&pixmap);
    pixmapPainter2.setRenderHint(QPainter::Antialiasing);
    pixmapPainter2.setCompositionMode(QPainter::CompositionMode_Clear);
    pixmapPainter2.setPen(Qt::transparent);
    pixmapPainter2.setBrush(Qt::transparent);
    pixmapPainter2.drawPath(rectPath);


    // 绘制阴影
    p.drawPixmap(this->rect(), pixmap, pixmap.rect());
}

void SearchResultPage::initUi()
{
    this->setFixedSize(700,552);
    m_hlayout = new QHBoxLayout(this);
    m_hlayout->setContentsMargins(18 ,18, 10, 18);

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
        sendResizeWidthSignal(656);
    });
    connect(m_resultArea, &ResultArea::keyPressChanged, m_detailArea, &DetailArea::setWidgetInfo);
    connect(m_resultArea, &ResultArea::keyPressChanged, this, [=] () {
        sendResizeWidthSignal(280);
    });
    connect(m_resultArea, &ResultArea::currentRowChanged, m_detailArea, &DetailArea::setWidgetInfo);
    connect(m_resultArea, &ResultArea::currentRowChanged, this, &SearchResultPage::currentRowChanged);
    connect(this, &SearchResultPage::currentRowChanged, m_resultArea, &ResultArea::clearSelectedRow);
    connect(m_resultArea, &ResultArea::resizeHeight, this, &SearchResultPage::resizeHeight);
    connect(this, &SearchResultPage::resizeWidth, m_resultArea, &ResultArea::resizeWidth);
    connect(m_resultArea, &ResultArea::rowClicked, this, [=] () {
        sendResizeWidthSignal(280);
    });
    connect(this, &SearchResultPage::setSelectionInfo, m_resultArea, &ResultArea::setSelectionInfo);
}

void SearchResultPage::setupConnectionsForWidget(ResultWidget *widget)
{
    connect(widget, &ResultWidget::currentRowChanged, m_detailArea, &DetailArea::setWidgetInfo);
    connect(widget, &ResultWidget::currentRowChanged, this, &SearchResultPage::currentRowChanged);
    connect(widget, &ResultWidget::currentRowChanged, this, [=] {
        QString pluginID = widget->pluginId();
        Q_EMIT this->setSelectionInfo(pluginID);
    });
    connect(this, &SearchResultPage::currentRowChanged, widget, &ResultWidget::clearSelectedRow);
    connect(widget, &ResultWidget::rowClicked, this, [=] () {
        sendResizeWidthSignal(280);
    });
    connect(this, &SearchResultPage::resizeWidth, widget, &ResultWidget::resizeWidth);
}
