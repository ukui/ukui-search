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

    this->setFocusPolicy(Qt::NoFocus);

    addTopControl();
    //加载默认视图
    this->setFixedWidth(Style::defaultMainViewWidWidth);
    m_topLayout->setContentsMargins(0,0,0,0);
    m_topLayout->setAlignment(m_queryLineEdit,Qt::AlignCenter);
}


/**
 * 监听treeview隐藏或显示
 */
void MainViewWidget::changesize()
{
    //文件模块
    if(fileNum == 0)
    {
        m_fileview->setVisible(false);
    } else {
        m_fileview->setVisible(true);
    }

    //设置模块
    if(SettingNum == 0){
        m_settingview->setVisible(false);
    }else{
        m_settingview->setVisible(true);
    }
    //应用模块
    if(appNum == 0){
        m_searchResultWid->setVisible(false);
    }else {
        m_searchResultWid->setVisible(true);
    }

}

/**
 * 添加顶部控件
 */
void MainViewWidget::addTopControl()
{
    m_topLayout=new UkuiSearchBarHLayout;
    m_topLayout->setSpacing(0);
    m_queryLineEdit=new QLineEdit;
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

    m_queryText=new QLabel;

//    m_queryText->setText(tr("Search"));

    m_queryText->adjustSize();
//    queryWidLayout->addWidget(m_queryText);
    m_queryLineEdit->setFocusPolicy(Qt::ClickFocus);
    m_queryLineEdit->installEventFilter(this);
    m_queryLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    m_queryLineEdit->setFixedSize(678,35);
    m_queryLineEdit->setMaxLength(100);


    //跑一个线程执行应用搜索
    m_searchAppThread=new SearchAppThread;
    m_searchFileThread=new SearchFileThread;
    connect(this,&MainViewWidget::sendSearchKeyword,
            m_searchFileThread,&SearchFileThread::recvSearchKeyword);

    connect(this,&MainViewWidget::sendSearchKeyword,
            m_searchAppThread,&SearchAppThread::recvSearchKeyword);

    connect(m_searchFileThread,&SearchFileThread::sendSearchResult,
            this,&MainViewWidget::recvFileSearchResult);

    connect(m_searchAppThread,&SearchAppThread::sendSearchResult,
            this,&MainViewWidget::recvSearchResult);


    //搜索应用
    connect(m_queryLineEdit, &QLineEdit::textChanged, this, &MainViewWidget::searchAppSlot);

    //把搜索的设置信息传入settingModel
    connect(m_queryLineEdit,&QLineEdit::textChanged,m_settingmodel,[=](const QString &search){
//            qDebug()<<"m_queryLineEdit"<<UkuiChineseLetter::getPinyins(search); // 中文转英文
            m_settingmodel->matchstart(search);

    });

    //网页搜索
    connect(m_queryLineEdit, &QLineEdit::textChanged, search_web_page,[=](){
        QString search=QString::fromLocal8Bit("   使用百度搜索").append(QString::fromLocal8Bit(" ")).append(QString::fromLocal8Bit("\"")).append(m_queryLineEdit->text()).append(QString::fromLocal8Bit("\""));
        search_web_page->setText(search);
        search1=m_queryLineEdit->text();
        //根据判断来隐藏与显示网页搜索
        if(search1 != QString::fromLocal8Bit("")){
            search_web_page->setVisible(true);
        } else {
            search_web_page->setVisible(false);
        }
    });

    //把搜索的文件信息传入settingModel
    connect(m_queryLineEdit,&QLineEdit::textChanged,m_filemodel,[=](const QString &search){
                m_filemodel->matchstart(search);
    });

    //监听点击事件，打开对应的设置选项
    connect(m_settingview,&QTreeView::clicked,this,[=](){
        m_settingmodel->run(m_settingview->currentIndex().row());
    });

    //监听点击事件，打开对应的文件
    connect(m_fileview,&QTreeView::clicked,this,[=](){
        m_filemodel->run(m_fileview->currentIndex().row(),m_fileview->currentIndex().column());
    });
}


/**
 * 搜索程序和文件槽函数
 */
void MainViewWidget::searchAppSlot(QString arg)
{
    Q_EMIT sendSearchKeyword(arg);
    m_searchAppThread->start();
    m_searchFileThread->start();
}

void MainViewWidget::recvSearchResult(QVector<QStringList> arg)
{
    m_searchAppThread->quit();
    m_searchResultWid->updateAppListView(arg);
}

void MainViewWidget::recvFileSearchResult(QStringList arg)
{
    m_searchFileThread->quit();
    m_filemodel->showResult(arg);
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
    m_topLayout->setContentsMargins(0,0,0,0);
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


    search1 = "";
    m_fileview =new fileview;
    m_settingview = new settingview;

    search_web_page = new websearch;
    search_web_page->setVisible(false);
    connect(search_web_page,&QPushButton::clicked,this,[=](){
           QString str = QString::fromLocal8Bit("https://www.baidu.com/baidu?tn=ubuntuu_cb&ie=utf-8&wd=").append(search1);
           QProcess p;
           p.setProgram(QString::fromLocal8Bit("chromium-browser"));
           p.setArguments(QStringList()<<str);
           p.startDetached(p.program(), p.arguments());
           p.waitForFinished(-1);
       });

    //初始化文件与设置view为隐藏
    m_searchResultWid=new SearchResultWidget;
    m_searchResultWid->setVisible(false);
    m_fileview->setVisible(false);
    m_settingview->setVisible(false);

    m_filemodel = new filemodel;
    m_settingmodel = new settingModel;

    //通过信号监听内容并设置宽度
    connect(m_settingmodel,&settingModel::requestUpdateSignal,m_settingview,&settingview::changesize);


    //通过信号监听内容并选择是否隐藏
    connect(m_searchResultWid,&SearchResultWidget::changeAppNum,this,&MainViewWidget::setAppView);
    connect(m_filemodel,&filemodel::requestUpdateSignal,this,&MainViewWidget::setFileView);
    connect(m_settingmodel,&settingModel::requestUpdateSignal,this,&MainViewWidget::setSettingView);


}
//添加搜索到的界面
void MainViewWidget::AddSearchWidget()
{
    m_fileview->setModel(m_filemodel);
    m_fileview->setColumnWidth(0,300);
    m_fileview->setColumnWidth(1,150);
    m_fileview->setColumnWidth(2,150);

    m_settingview->setModel(m_settingmodel);

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

void MainViewWidget::setFileView(int row)
{
    fileNum=row;
    changesize();
}

void MainViewWidget::setSettingView(int row)
{
    SettingNum=row;
    changesize();
}

void MainViewWidget::setAppView(int row)
{
    appNum=row;
    changesize();

}

void MainViewWidget::searchContent(QString searchcontent){
    m_queryLineEdit->setText(searchcontent);
}
