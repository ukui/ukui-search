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
#include "stacked-widget.h"

using namespace Zeeker;
Zeeker::StackedWidget::StackedWidget(QWidget *)
{
    this->initWidgets();
    this->initConnections();
}

Zeeker::StackedWidget::~StackedWidget()
{
    if (m_homePage) {
        delete m_homePage;
        m_homePage = NULL;
    }
    if (m_searchPage) {
        delete m_searchPage;
        m_searchPage = NULL;
    }
}

/**
 * @brief StackedWidget::setPage 设置StackedWidget所显示页面
 * @param type 页面类型
 */
void StackedWidget::setPage(const int & type)
{
    this->setCurrentIndex(type);
}

int StackedWidget::currentPage()
{
    return this->currentIndex();
}

/**
 * @brief StackedWidget::setPlugins 设置所有插件
 */
void StackedWidget::setPlugins(const QStringList &plugins)
{
    m_searchPage->setPlugins(plugins);
}

/**
 * @brief StackedWidget::addPlugin 添加一个插件
 */
void StackedWidget::appendPlugin(const QString &plugin)
{
    m_searchPage->appendPlugin(plugin);
}

/**
 * @brief StackedWidget::initWidgets 初始化向stackedwidget添加窗口
 */
void StackedWidget::initWidgets()
{
    //NEW_TODO
//    m_homePage = new HomePage;
//    this->insertWidget(int(StackedPage::HomePage), m_homePage);
//    this->setPage(int(StackedPage::HomePage));

    m_searchPage = new SearchPage;
    m_searchPage->setSize(this->width(), this->height());
    this->insertWidget(int(StackedPage::SearchPage), m_searchPage);
}

void StackedWidget::initConnections()
{
    connect(this, &StackedWidget::startSearch, m_searchPage, &SearchPage::startSearch);
    connect(this, &StackedWidget::stopSearch, m_searchPage, &SearchPage::stopSearch);
    connect(m_searchPage, &SearchPage::effectiveSearch, this, &StackedWidget::effectiveSearch);
}
