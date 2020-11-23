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
    search1 = QString::fromLocal8Bit("");
    m_fileview =new QTreeView;
    m_settingview = new QTreeView;

    search_web_page = new QPushButton;

    search_web_page->setStyleSheet("QPushButton{background-color:rgba(0,0,0,100%);color: white;border-radius: 10px;border: 1px white;text-align: left;}");


    search_web_page->setText("   请使用百度搜索");
    search_web_page->setFixedHeight(40);

    search_web_page->setVisible(false);
    connect(search_web_page,&QPushButton::clicked,this,[=](){
           QString str = QString::fromLocal8Bit("https://www.baidu.com/baidu?tn=ubuntuu_cb&ie=utf-8&wd=").append(search1);
           QProcess p;
           p.setProgram(QString::fromLocal8Bit("chromium-browser"));
           p.setArguments(QStringList()<<str);
           p.startDetached(p.program(), p.arguments());
           p.waitForFinished(-1);
       });

    //初始化搜索的配置，包括信号监听
    initSearchWidget();
    //初始化ui
    initUi();
}

MainViewWidget::~MainViewWidget()
{
    delete m_animation;
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
    m_topWidget=new QWidget;

    mainLayout->addWidget(m_topWidget);
    this->setLayout(mainLayout);

    this->setFocusPolicy(Qt::NoFocus);

    addTopControl();
    //加载默认视图
    this->setFixedSize(Style::defaultMainViewWidWidth,Style::minh);
    m_topWidget->setFixedSize(30,30);
    m_topLayout->setContentsMargins(0,0,0,0);
    m_topLayout->setAlignment(m_queryLineEdit,Qt::AlignCenter);
    m_queryLineEdit->setFixedSize(Style::defaultQueryLineEditWidth,30);
    m_queryText->adjustSize();
}


/**
 * 监听treeview改变窗口大小
 */
void MainViewWidget::changesize()
{

//    if(fileNum==0)
//    {
//        m_fileview->setVisible(false);
//    } else {
//        if(fileNum>10){
//            m_fileview->setVisible(true);
//            m_fileview->setFixedSize(Style::defaultMainViewWidWidth,5*60);
//        } else {
//            m_fileview->setVisible(true);
//            m_fileview->setFixedSize(Style::defaultMainViewWidWidth,fileNum);
//        }
//    }

//    if(SettingNum==0){
//        m_settingview->setVisible(false);
//    }else{
//        m_settingview->setVisible(true);
//         m_settingview->setFixedSize(Style::defaultMainViewWidWidth,SettingNum*60);
//    }

}

/**
 * 添加顶部控件
 */
void MainViewWidget::addTopControl()
{
    m_topLayout=new QHBoxLayout;
    m_topLayout->setSpacing(0);
    m_queryLineEdit=new QLineEdit;
//    char style[100];
//    sprintf(style, "QLineEdit{border:0px;background-color:%s;border-radius:4px;}",QueryLineEditBackground);
//    m_queryLineEdit->setStyleSheet(style);
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
    m_queryWid=new QWidget;
    m_queryWid->setParent(m_queryLineEdit);
    m_queryWid->setFocusPolicy(Qt::NoFocus);
//    m_queryWid->setStyleSheet("border:10px;background:transparent;#ff0000;");

    //queryWidLayout 搜索图标和文字所在的布局
    QHBoxLayout* queryWidLayout=new QHBoxLayout;
    queryWidLayout->setContentsMargins(5,3,0,2);
    queryWidLayout->setSpacing(5);
    m_queryWid->setLayout(queryWidLayout);
    //输入框的搜索图标，不清楚loadSvg为什么不能用了
    QPixmap pixmap/*=loadSvg(QString(":/data/img/mainviewwidget/search.svg"),16)*/;

//    QGSettings gsetting(QString("org.ukui.style").toLocal8Bit());
//    if(gsetting.get("style-name").toString()=="ukui-light")//反黑
//        pixmap=drawSymbolicBlackColoredPixmap(pixmap);
//    else
//        pixmap=drawSymbolicColoredPixmap(pixmap);//反白
//    pixmap.setDevicePixelRatio(qApp->devicePixelRatio());

    m_queryIcon=new QLabel;
    m_queryIcon->setFixedSize(pixmap.size());
    m_queryIcon->setPixmap(pixmap);
    m_queryText=new QLabel;
    m_queryText->setText(tr("Search"));
    m_queryText->adjustSize();
    queryWidLayout->addWidget(m_queryIcon);
    queryWidLayout->addWidget(m_queryText);
    m_queryLineEdit->setFocusPolicy(Qt::ClickFocus);
    m_queryLineEdit->installEventFilter(this);
    m_queryLineEdit->setContextMenuPolicy(Qt::NoContextMenu);

    //点击搜索框的动画效果
    m_animation= new QPropertyAnimation(m_queryWid,"geometry");
    m_animation->setDuration(100);
    connect(m_animation,&QPropertyAnimation::finished,this,&MainViewWidget::animationFinishedSlot);

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

    connect(m_queryLineEdit, &QLineEdit::textChanged, this, &MainViewWidget::searchAppSlot);


    connect(m_queryLineEdit,&QLineEdit::textChanged,m_settingmodel,[=](const QString &search){
            m_settingmodel->matchstart(search);
//            startmatchTimer->start();
    });

    //网页搜索
    connect(m_queryLineEdit, &QLineEdit::textChanged, search_web_page,[=](){
        QString search=QString::fromLocal8Bit("   使用百度搜索").append(QString::fromLocal8Bit(" ")).append(QString::fromLocal8Bit("\"")).append(m_queryLineEdit->text()).append(QString::fromLocal8Bit("\""));
//        QString search = QString::fromLocal8Bit(QString("使用百度搜索 1%2%3%").arg(QString::fromLocal8Bit("")).arg(input->text().arg(QString::fromLocal8Bit(""))));
        search_web_page->setText(search);
        search1=m_queryLineEdit->text();
        //根据判断来隐藏与显示网页搜索
        if(search1 != QString::fromLocal8Bit("")){
            search_web_page->setVisible(true);
        } else {
            search_web_page->setVisible(false);
        }
    });


    connect(m_queryLineEdit,&QLineEdit::textChanged,m_filemodel,[=](const QString &search){
                m_filemodel->matchstart(search);
    });

    connect(m_settingview,&QTreeView::clicked,this,[=](){
        m_settingmodel->run(m_settingview->currentIndex().row());
    });

    connect(m_fileview,&QTreeView::clicked,this,[=](){
        m_filemodel->run(m_fileview->currentIndex().row(),m_fileview->currentIndex().column());
    });
}

/*事件过滤*/
bool MainViewWidget::eventFilter(QObject *watched, QEvent *event)
{
#if 1
    if(watched==m_queryLineEdit)
    {
        if(event->type()==QEvent::FocusIn)
        {
            if(!m_queryLineEdit->text().isEmpty())
            {
                if(m_searchKeyWords.isEmpty())
                    searchAppSlot(m_queryLineEdit->text());
            }
            else
            {
                m_animation->stop();
                m_animation->setStartValue(QRect((m_queryLineEdit->width()-(m_queryIcon->width()+m_queryText->width()+10))/2,0,
                                                 m_queryIcon->width()+m_queryText->width()+10,0));
                m_animation->setEndValue(QRect(0,0,
                                               m_queryIcon->width()+5,0));
                m_animation->setEasingCurve(QEasingCurve::OutQuad);
                m_animation->start();
                m_queryLineEdit->setTextMargins(-5,1,0,1);
            }
            m_isSearching=true;
        }
        else if(event->type()==QEvent::FocusOut)
        {
            m_searchKeyWords.clear();
            if(m_queryLineEdit->text().isEmpty())
            {
                if(m_isSearching)
                {
                    m_animation->stop();
                    m_queryText->adjustSize();
                    m_animation->setStartValue(QRect(0,0,
                                                     m_queryIcon->width()+5,0));
                    m_animation->setEndValue(QRect((m_queryLineEdit->width()-(m_queryIcon->width()+m_queryText->width()+10))/2,0,
                                                   m_queryIcon->width()+m_queryText->width()+10,0));
                    m_animation->setEasingCurve(QEasingCurve::InQuad);
                    m_animation->start();
                }
            }
            m_isSearching=false;
        }
    }

    return QWidget::eventFilter(watched,event);     // 最后将事件交给上层对话框
#endif
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

/*
 * 点击搜索框的动画效果
*/
void MainViewWidget::animationFinishedSlot()
{
    if(m_isSearching)
    {
        m_queryWid->layout()->removeWidget(m_queryText);
        m_queryText->setParent(nullptr);
        m_queryLineEdit->setTextMargins(20,1,0,1);
        if(!m_searchKeyWords.isEmpty())
        {
            m_queryLineEdit->setText(m_searchKeyWords);
            m_searchKeyWords.clear();
        }
    }
    else
        m_queryWid->layout()->addWidget(m_queryText);
}

/**
 * 加载默认主视图
 * 在加载主视图的时候，主视图上需要有搜索到的应用程序，搜索到的文件，搜索到的控制面板选项等
 * 采用默认加载全部视图，各个搜索结果的视图的高度根据搜索结果进行自适应，默认高度均为0
 */
void MainViewWidget::loadMinMainView()
{
    this->setFixedSize(Style::defaultMainViewWidWidth,Style::minh);
    m_topWidget->setFixedSize(Style::defaultMainViewWidWidth,Style::defaultTopWidHeight);
    m_topLayout->setContentsMargins(0,0,0,0);
    m_topLayout->setAlignment(m_queryLineEdit,Qt::AlignCenter);
    m_queryLineEdit->setFixedSize(Style::defaultQueryLineEditWidth,30);
    if(m_queryLineEdit->text().isEmpty())
    {
        if(m_queryWid->layout()->count()==1)
            m_queryWid->layout()->addWidget(m_queryText);
        m_queryText->adjustSize();
    }

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
    //初始化文件与设置view为隐藏
//    m_fileview->setVisible(false);
//    m_settingview->setVisible(false);

    m_filemodel = new filemodel;
    m_settingmodel = new settingModel;

    //通过信号监听内容并设置宽度
    connect(m_filemodel,&filemodel::requestUpdateSignal,this,&MainViewWidget::setFileView);
    connect(m_settingmodel,&settingModel::requestUpdateSignal,this,&MainViewWidget::setSettingView);

    m_searchResultWid=new SearchResultWidget;
}
//添加搜索到的界面
void MainViewWidget::AddSearchWidget()
{
    m_fileview->setModel(m_filemodel);
    m_fileview->setColumnWidth(0,300);
    m_fileview->setColumnWidth(1,150);
    m_fileview->setColumnWidth(2,150);
    m_settingview->setModel(m_settingmodel);

    //添加已经安装的应用界面
    mainLayout->addWidget(m_searchResultWid);
    //添加文件搜索界面
    mainLayout->addWidget(m_fileview);
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
