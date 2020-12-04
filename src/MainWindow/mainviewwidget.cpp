/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */

#include "mainviewwidget.h"
#include "src/Style/style.h"
#include <QSvgRenderer>
#include <QPainter>
#include <libbamf/bamf-matcher.h>
#include <syslog.h>
#include <QDebug>

/**
 * @brief MainViewWidget界面
 * @param parent
 * 初始化ui：initUi
 * initSearchWidget ：初始化搜索的配置，包括信号监听
 */
MainViewWidget::MainViewWidget(QWidget *parent) :
    QWidget(parent)
{
    initSearchWidget();
    initUi();
}

MainViewWidget::~MainViewWidget()
{
    delete m_searchAppThread;
}

/**
 * @brief initUi MainViewWidget 初始化
 * 添加搜索框所在的界面
 * 加载默认视图
 */
void MainViewWidget::initUi()
{
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_StyledBackground,true);
    this->setAttribute(Qt::WA_TranslucentBackground);

    mainLayout=new QVBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    m_topWidget=new UKuiSeachBarWidget;

    mainLayout->addWidget(m_topWidget);
    this->setLayout(mainLayout);

    addTopControl();
    this->setFixedWidth(Style::defaultMainViewWidWidth);
}

/**
 * 添加顶部控件
 */
void MainViewWidget::addTopControl()
{
    m_topLayout=new UkuiSearchBarHLayout;
    m_topWidget->setLayout(m_topLayout);
}

/**
 * 加载默认主视图
 * 在加载主视图的时候，主视图上需要有搜索到的应用程序，搜索到的文件，搜索到的控制面板选项等
 * 采用默认加载全部视图，各个搜索结果的视图的高度根据搜索结果进行自适应，默认高度均为0
 */
void MainViewWidget::loadMinMainView()
{
    this->setFixedWidth(Style::defaultMainViewWidWidth);
    AddSearchWidget();
}

/**
 * @brief initSearchWidget 搜索到的界面初始化
 * 包括应用搜索，文件搜索，设置搜索
 */
void MainViewWidget::initSearchWidget()
{
    m_fileview    = new SearchFileWidget;
    m_settingview = new SettingWidget;
    search_web_page   = new websearch;
    m_searchResultWid = new SearchResultWidget;
}

/**
 * @brief AddSearchWidget  添加各个搜索界面 包括：
 * 添加文件搜索界面
 * 添加已经安装的应用界面
 * 添加控制面板搜索项目界面
 * 网页搜索界面缩因子
 * 添加伸
 */
void MainViewWidget::AddSearchWidget()
{
    mainLayout->addItem(new QSpacerItem(10,5));
    mainLayout->addWidget(m_fileview);
    mainLayout->addItem(new QSpacerItem(10,5));
    mainLayout->addWidget(m_searchResultWid);
    mainLayout->addItem(new QSpacerItem(10,5));
    mainLayout->addWidget(m_settingview);
    mainLayout->addItem(new QSpacerItem(10,5));
    mainLayout->addWidget(search_web_page);
    mainLayout->addStretch();
}

void MainViewWidget::searchContent(QString searchcontent){
    m_topLayout->searchContent(searchcontent);
}
