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

/* MainViewWidget界面
 * 包含 m_queryLineEdit 搜索框界面
 * searchResultWidget 应用搜索结果界面
 * m_queryLineEdit ：QLineEdit搜索框
 * m_topWidget  顶部搜索框所在的界面
 *
*/
MainViewWidget::MainViewWidget(QWidget *parent) :
    QWidget(parent)
{
    //初始化搜索的配置，包括信号监听
    initSearchWidget();

    //初始化ui
    initUi();

    /*发送输入框文字改变的dbus*/
    QDBusConnection::sessionBus().unregisterService("org.ukui.search.service");
    QDBusConnection::sessionBus().registerService("org.ukui.search.service");
    QDBusConnection::sessionBus().registerObject("/lineEdit/textChanged", this,QDBusConnection :: ExportAllSlots | QDBusConnection :: ExportAllSignals);
}

MainViewWidget::~MainViewWidget()
{
    delete m_searchAppThread;
}

/*
 * MainViewWidget 初始化
 * QWidget *m_queryWid   -> QLineEdit *m_queryLineEdit
*/
void MainViewWidget::initUi()
{
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_StyledBackground,true);
    this->setAttribute(Qt::WA_TranslucentBackground);

    mainLayout=new QVBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    //顶部搜索框的widget
    m_topWidget=new UKuiSeachBarWidget;

    mainLayout->addWidget(m_topWidget);
    this->setLayout(mainLayout);

//    this->setFocusPolicy(Qt::NoFocus);

    addTopControl();
    //加载默认视图
    this->setFixedWidth(Style::defaultMainViewWidWidth);
    m_topLayout->setContentsMargins(0,0,0,0);
    m_topLayout->setAlignment(m_queryLineEdit,Qt::AlignCenter);
}

/**
 * 添加顶部控件
 */
void MainViewWidget::addTopControl()
{
    m_topLayout=new UkuiSearchBarHLayout;
    m_topLayout->setSpacing(0);
    m_queryLineEdit=new UKuiSearchLineEdit;
    m_topLayout->addWidget(m_queryLineEdit);
    m_topWidget->setLayout(m_topLayout);

    initQueryLineEdit();

}

/**
 * 添加搜索框  m_queryWid -> queryWidLayout -> m_queryIcon + m_queryText
 */
void MainViewWidget::initQueryLineEdit()
{
    //搜索框ui
    m_queryWid=new UKuiSeachBar;
    m_queryWid->setParent(m_queryLineEdit);

    //queryWidLayout 搜索图标和文字所在的布局
    QHBoxLayout* queryWidLayout=new QHBoxLayout;
    m_queryWid->setLayout(queryWidLayout);

    //输入框文本更新
    connect(m_queryLineEdit, &QLineEdit::textChanged, this, &MainViewWidget::lineEditTextChanged);

}

void MainViewWidget::lineEditTextChanged(QString arg)
{
    /*创建QT的DBus信号*/
    QDBusMessage message =QDBusMessage::createSignal("/lineEdit/textChanged", "org.ukui.search.inputbox", "InputBoxTextChanged");
    message<<arg;
    /*
     * 需要此点击信号的应用需要做如下绑定
     * QDBusConnection::sessionBus().connect(QString(), QString("/lineEdit/textChanged"), "org.ukui.search.inputbox", "InputBoxTextChanged", this, SLOT(client_get(QString)));
     * 在槽函数client_get(void)　中处理接受到的点击信号
　　　*/
    QDBusConnection::sessionBus().send(message);
}

/**
 * 加载默认主视图
 * 在加载主视图的时候，主视图上需要有搜索到的应用程序，搜索到的文件，搜索到的控制面板选项等
 * 采用默认加载全部视图，各个搜索结果的视图的高度根据搜索结果进行自适应，默认高度均为0
 */
void MainViewWidget::loadMinMainView()
{
    this->setFixedWidth(Style::defaultMainViewWidWidth);
    m_topWidget->setFixedSize(Style::defaultMainViewWidWidth,Style::defaultTopWidHeight);
    m_topLayout->setAlignment(m_queryLineEdit,Qt::AlignCenter);

    QLayoutItem* child;
    if((child=this->layout()->takeAt(1))!=nullptr)
    {
        QWidget* childWid=child->widget();
        if(childWid!=nullptr)
        {
            this->layout()->removeWidget(childWid);
            childWid->setParent(nullptr);
        }
    }

    //所有的搜索界面添加入口
    AddSearchWidget();
}

//搜索到的界面（包括应用搜索，文件搜索，设置搜索）的初始化
void MainViewWidget::initSearchWidget()
{
    m_fileview    = new SearchFileWidget;
    m_settingview = new SettingWidget;

    search_web_page   = new websearch;
    m_searchResultWid = new SearchResultWidget;


}
//添加搜索到的界面
void MainViewWidget::AddSearchWidget()
{

    //添加文件搜索界面
    mainLayout->addWidget(m_fileview);

    //添加已经安装的应用界面
    mainLayout->addWidget(m_searchResultWid);

    //添加控制面板搜索项目界面
    mainLayout->addWidget(m_settingview);

    //网页搜索界面
    mainLayout->addWidget(search_web_page);

    //添加伸缩因子
    mainLayout->addStretch();
}


/*
 * 鼠标点击窗口外部事件
 * 会触发MainWindow::event
 * 调用widgetMakeZero，执行清空输入框的操作
*/
void MainViewWidget::widgetMakeZero()
{
    m_queryLineEdit->clear();
    m_queryLineEdit->clearFocus();
    m_queryLineEdit->setTextMargins(0,1,0,1);
}

void MainViewWidget::searchContent(QString searchcontent){
    m_queryLineEdit->setText(searchcontent);
}
