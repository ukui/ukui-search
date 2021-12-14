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
#include "web-search-view.h"
#define MAIN_MARGINS 0,0,0,0
#define MAIN_SPACING 0
#define TITLE_HEIGHT 30
#define VIEW_ICON_SIZE 24

using namespace UkuiSearch;
WebSearchView::WebSearchView(QWidget *parent) : QTreeView(parent)
{
    this->setFrameShape(QFrame::NoFrame);
    this->viewport()->setAutoFillBackground(false);
    this->setRootIsDecorated(false);
    this->setIconSize(QSize(VIEW_ICON_SIZE, VIEW_ICON_SIZE));
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setHeaderHidden(true);
    m_model = new WebSearchModel(this);
    this->setModel(m_model);
    m_style_delegate = new ResultViewDelegate(this);
    this->setItemDelegate(m_style_delegate);
}

bool WebSearchView::isSelected()
{
    return m_is_selected;
}

int WebSearchView::showHeight()
{
    return this->rowHeight(this->model()->index(0, 0, QModelIndex()));
}

QModelIndex WebSearchView::getModlIndex(int row, int column)
{
    return this->m_model->index(row, column);
}

void WebSearchView::clearSelectedRow()
{
    if (!m_is_selected) {
        this->blockSignals(true);
        //this->clearSelection();
        this->setCurrentIndex(QModelIndex());
        this->blockSignals(false);
    }
}

void WebSearchView::startSearch(const QString & keyword)
{
    this->m_style_delegate->setSearchKeyword(keyword);
    this->m_model->startSearch(keyword);
    m_keyWord = keyword;
}

void WebSearchView::mouseReleaseEvent(QMouseEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if (!index.isValid()) {
        this->clearSelection();
    }
    return QTreeView::mouseReleaseEvent(event);
}

void WebSearchView::LaunchBrowser()
{
    QString address;
    QString engine = GlobalSettings::getInstance()->getValue("web_engine").toString();
    if(!engine.isEmpty()) {
        if(engine == "360") {
            address = "https://so.com/s?q=" + m_keyWord; //360
        } else if(engine == "sougou") {
            address = "https://www.sogou.com/web?query=" + m_keyWord; //搜狗
        } else {
            address = "http://baidu.com/s?word=" + m_keyWord; //百度
        }
    } else { //默认值
        address = "http://baidu.com/s?word=" + m_keyWord ; //百度
    }
    QDesktopServices::openUrl(address);
}

void WebSearchView::initConnections()
{

}

WebSearchWidget::WebSearchWidget(QWidget *parent) : QWidget(parent)
{
    this->initUi();
    initConnections();
}

QString WebSearchWidget::getWidgetName()
{
    return m_titleLabel->text();
}

void WebSearchWidget::setEnabled(const bool &enabled)
{
    m_enabled = enabled;
}

void WebSearchWidget::clearResultSelection()
{
    this->m_webSearchView->setCurrentIndex(QModelIndex());
}

QModelIndex WebSearchWidget::getModlIndex(int row, int column)
{
    return this->m_webSearchView->getModlIndex(row, column);
}

void WebSearchWidget::setResultSelection(const QModelIndex &index)
{
    this->m_webSearchView->setCurrentIndex(index);
}

void WebSearchWidget::LaunchBrowser()
{
    this->m_webSearchView->LaunchBrowser();
}

void WebSearchWidget::initUi()
{
    m_mainLyt = new QVBoxLayout(this);
    this->setLayout(m_mainLyt);
    m_mainLyt->setContentsMargins(MAIN_MARGINS);
    m_mainLyt->setSpacing(MAIN_SPACING);

    m_titleLabel = new TitleLabel(this);
    m_titleLabel->setText(tr("Web Page"));
    m_titleLabel->setFixedHeight(TITLE_HEIGHT);

    m_webSearchView = new WebSearchView(this);

    m_mainLyt->addWidget(m_titleLabel);
    m_mainLyt->addWidget(m_webSearchView);
    this->setFixedHeight(m_webSearchView->height() + TITLE_HEIGHT);
    this->setFixedWidth(656);
}

void WebSearchWidget::initConnections()
{
    connect(this, &WebSearchWidget::startSearch, m_webSearchView, &WebSearchView::startSearch);
    connect(m_webSearchView, &WebSearchView::clicked, this, [=] () {
        this->LaunchBrowser();
    });
}
