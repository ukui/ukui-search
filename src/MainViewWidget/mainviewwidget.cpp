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

/*MainViewWidget界面
 * 包含 m_queryLineEdit 搜索框界面
 * searchResultWidget 应用搜索结果界面
 * m_queryLineEdit ：QLineEdit搜索框
 *
*/
MainViewWidget::MainViewWidget(QWidget *parent) :
    QWidget(parent)
{


    m_fileview =new QTreeView;
    m_settingview = new QTreeView;

//    m_fileview->setStyleSheet("QTreeView { background-color : white; border: 0px none;border-radius: 10px;}");
//    m_settingview->setStyleSheet("QTreeView { background-color : white; border: 0px none;border-radius: 10px;}");

    //初始化文件与设置view为隐藏
    m_fileview->setVisible(false);
    m_settingview->setVisible(false);

    m_filemodel = new filemodel;
    m_settingmodel = new settingModel;


    //通过信号监听内容并设置宽度
    connect(m_filemodel,&filemodel::requestUpdateSignal,this,&MainViewWidget::setFileView);

    connect(m_settingmodel,&settingModel::requestUpdateSignal,this,&MainViewWidget::setSettingView);

    initUi();
}

MainViewWidget::~MainViewWidget()
{
    delete m_ukuiMenuInterface;
    delete m_directoryChangedThread;
    delete m_animation;
    delete m_searchAppThread;
}

void MainViewWidget::initUi()
{
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_StyledBackground,true);
    this->setStyleSheet("border:0px;background:transparent;");

    QVBoxLayout* mainLayout=new QVBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    //顶部搜索框的widget
    m_topWidget=new QWidget;
//    m_topWidget->setStyleSheet("border:0px;background:transparent;");

    m_verticalSpacer=new QSpacerItem(20,40, QSizePolicy::Fixed, QSizePolicy::Expanding);
    mainLayout->addWidget(m_topWidget);
    mainLayout->addItem(m_verticalSpacer);
    this->setLayout(mainLayout);

    this->setFocusPolicy(Qt::NoFocus);

    m_searchResultWid=new SearchResultWidget;
    m_ukuiMenuInterface=new UkuiMenuInterface;

    connect(this,&MainViewWidget::sendDirectoryPath,m_directoryChangedThread,&DirectoryChangedThread::recvDirectoryPath);
    connect(m_directoryChangedThread,&DirectoryChangedThread::requestUpdateSignal,this,&MainViewWidget::requestUpdateSlot);
    //发送隐藏主界面信号
    connect(m_searchResultWid,&SearchResultWidget::sendHideMainWindowSignal,this,&MainViewWidget::sendHideMainWindowSignal);

    addTopControl();
    //加载默认视图
    //    loadMinMainView();
    this->setFixedSize(Style::defaultMainViewWidWidth,Style::minh);
    m_topWidget->setFixedSize(Style::defaultMainViewWidWidth,Style::defaultTopWidHeight);
    m_topLayout->setContentsMargins(0,0,0,0);
    m_topLayout->setAlignment(m_queryLineEdit,Qt::AlignCenter);
    m_queryLineEdit->setFixedSize(Style::defaultQueryLineEditWidth,Style::defaultQueryLineEditHeight);
    m_queryText->adjustSize();
    m_queryWid->setGeometry(QRect((m_queryLineEdit->width()-(m_queryIcon->width()+m_queryText->width()+10))/2,0,
                                  m_queryIcon->width()+m_queryText->width()+10,Style::QueryLineEditHeight));
    m_queryWid->show();

//    mainLayout->addWidget(m_searchResultWid);
//    mainLayout->addWidget(m_fileview);
//    mainLayout->addWidget(m_settingview);
//    m_fileview->setModel(m_filemodel);
//    m_settingview->setModel(m_settingmodel);

    mainLayout->insertWidget(1,m_searchResultWid);
//    mainLayout->addWidget(m_searchResultWid);
//    layout->insertWidget(1,m_searchResultWid);
//    layout->insertWidget(2,m_fileview);
//    layout->insertWidget(3,m_settingview);

}


/**
 * 监听treeview改变窗口大小
 */
void MainViewWidget::changesize()
{

    if(fileNum==0)
    {
        m_fileview->setVisible(false);
    } else {
        if(fileNum>10){
            m_fileview->setVisible(true);
            m_fileview->setFixedSize(Style::defaultMainViewWidWidth,5*60);
        } else {
            m_fileview->setVisible(true);
            m_fileview->setFixedSize(Style::defaultMainViewWidWidth,fileNum);
        }
    }

    if(SettingNum==0){
        m_settingview->setVisible(false);
    }else{
        m_settingview->setVisible(true);
         m_settingview->setFixedSize(Style::defaultMainViewWidWidth,SettingNum*60);
    }

}

/**
 * 添加顶部控件
 */
void MainViewWidget::addTopControl()
{
    m_topLayout=new QHBoxLayout;
    m_topLayout->setSpacing(0);
    m_queryLineEdit=new QLineEdit;
    char style[100];
    sprintf(style, "QLineEdit{border:0px;background-color:%s;border-radius:4px;}",QueryLineEditBackground);
    m_queryLineEdit->setStyleSheet(style);
    m_topLayout->addWidget(m_queryLineEdit);
    m_topWidget->setLayout(m_topLayout);

    initQueryLineEdit();

}

/**
 * 添加搜索框
 */
void MainViewWidget::initQueryLineEdit()
{
    //搜索框ui
    m_queryWid=new QWidget;
    m_queryWid->setParent(m_queryLineEdit);
    m_queryWid->setFocusPolicy(Qt::NoFocus);
    m_queryWid->setStyleSheet("border:0px;background:transparent");
    QHBoxLayout* queryWidLayout=new QHBoxLayout;
    queryWidLayout->setContentsMargins(5,0,0,0);
    queryWidLayout->setSpacing(5);
    m_queryWid->setLayout(queryWidLayout);
    //输入框的搜索图标，不清楚loadSvg为什么不能用了
    QPixmap pixmap/*=loadSvg(QString(":/data/img/mainviewwidget/search.svg"),16)*/;
    m_queryIcon=new QLabel;
    m_queryIcon->setStyleSheet("background:transparent");
    m_queryIcon->setFixedSize(pixmap.size());
    m_queryIcon->setPixmap(pixmap);
    m_queryText=new QLabel;
    m_queryText->setText(tr("Search"));
    m_queryText->setStyleSheet("background:transparent;color:#626c6e;");
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
    connect(this,&MainViewWidget::sendSearchKeyword,
            m_searchAppThread,&SearchAppThread::recvSearchKeyword);
    connect(m_searchAppThread,&SearchAppThread::sendSearchResult,
            this,&MainViewWidget::recvSearchResult);
    connect(m_queryLineEdit, &QLineEdit::textChanged, this, &MainViewWidget::searchAppSlot);

    connect(m_queryLineEdit,&QLineEdit::textChanged,m_settingmodel,[=](const QString &search){
            m_settingmodel->matchstart(search);
//            startmatchTimer->start();

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
    if(watched==m_queryLineEdit)
    {
        if(event->type()==QEvent::FocusIn)
        {
            char style[200];
            sprintf(style, "QLineEdit{border:1px solid %s;background-color:%s;border-radius:4px;color:#ffffff;}",
                    QueryLineEditClickedBorder,QueryLineEditClickedBackground);
            m_queryLineEdit->setStyleSheet(style);
            if(!m_queryLineEdit->text().isEmpty())
            {
                if(m_searchKeyWords.isEmpty())
                    searchAppSlot(m_queryLineEdit->text());
            }
            else
            {
                m_animation->stop();
                m_animation->setStartValue(QRect((m_queryLineEdit->width()-(m_queryIcon->width()+m_queryText->width()+10))/2,0,
                                                 m_queryIcon->width()+m_queryText->width()+10,Style::QueryLineEditHeight));
                m_animation->setEndValue(QRect(0,0,
                                               m_queryIcon->width()+5,Style::QueryLineEditHeight));
                m_animation->setEasingCurve(QEasingCurve::OutQuad);
                m_animation->start();
                //                 m_queryLineEdit->setReadOnly(true);
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
                    char style[100];
                    sprintf(style, "QLineEdit{border:0px;background-color:%s;border-radius:4px;}",QueryLineEditBackground);
                    m_animation->stop();
                    m_queryLineEdit->setStyleSheet(style);
                    m_queryText->adjustSize();
                    m_animation->setStartValue(QRect(0,0,
                                                     m_queryIcon->width()+5,Style::QueryLineEditHeight));
                    m_animation->setEndValue(QRect((m_queryLineEdit->width()-(m_queryIcon->width()+m_queryText->width()+10))/2,0,
                                                   m_queryIcon->width()+m_queryText->width()+10,Style::QueryLineEditHeight));
                    m_animation->setEasingCurve(QEasingCurve::InQuad);
                    m_animation->start();
                }
            }
            else {
                char style[100];
                sprintf(style, "QLineEdit{border:0px;background-color:%s;border-radius:4px;color:#ffffff;}",
                        QueryLineEditBackground);
                m_queryLineEdit->setStyleSheet(style);
            }
            m_isSearching=false;
        }
    }

    return QWidget::eventFilter(watched,event);     // 最后将事件交给上层对话框
}

/**
 * 搜索程序和文件槽函数
 */
void MainViewWidget::searchAppSlot(QString arg)
{
    Q_EMIT sendSearchKeyword(arg);
    m_searchAppThread->start();
}

void MainViewWidget::recvSearchResult(QVector<QStringList> arg)
{
    m_searchAppThread->quit();
    m_searchResultWid->updateAppListView(arg);
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
    m_queryLineEdit->setFixedSize(Style::defaultQueryLineEditWidth,Style::defaultQueryLineEditHeight);
    if(m_queryLineEdit->text().isEmpty())
    {
        if(m_queryWid->layout()->count()==1)
            m_queryWid->layout()->addWidget(m_queryText);
        m_queryText->adjustSize();
        m_queryWid->setGeometry(QRect((m_queryLineEdit->width()-(m_queryIcon->width()+m_queryText->width()+10))/2,0,
                                      m_queryIcon->width()+m_queryText->width()+10,Style::QueryLineEditHeight));
        m_queryWid->show();
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
    QVBoxLayout *layout=qobject_cast<QVBoxLayout*>(this->layout());
//    layout->addWidget(m_searchResultWid);
    layout->insertWidget(1,m_searchResultWid);

    m_fileview->setModel(m_filemodel);
    layout->insertWidget(2,m_fileview);
//    layout->addWidget(m_fileview);

    m_settingview->setModel(m_settingmodel);
    layout->insertWidget(3,m_settingview);
//    layout->addWidget(m_settingview);


}

void MainViewWidget::resizeControl()
{
    if(m_isFullScreen)
    {
        //        if(m_widgetState==2)
        //            m_fullLetterWid->setLetterBtnGeometry();
        //        if(m_widgetState==3)
        //            m_fullFunctionWid->setFunctionBtnGeometry();

        this->setFixedSize(Style::MainViewWidWidth,
                           Style::heightavailable);
        m_topWidget->setFixedSize(this->width(),Style::TopWidgetHeight);
        m_queryLineEdit->setFixedSize(Style::QueryLineEditWidth,Style::QueryLineEditHeight);

        m_topLayout->setContentsMargins((m_topWidget->width()-Style::LeftWidWidth-m_queryLineEdit->width())/2+Style::LeftWidWidth,
                                        0,
                                        (m_topWidget->width()-Style::LeftWidWidth-m_queryLineEdit->width())/2,
                                        0);
    }
    else
    {
        this->setFixedSize(Style::defaultMainViewWidWidth,Style::minh);
        m_topWidget->setFixedSize(Style::defaultMainViewWidWidth,Style::defaultTopWidHeight);
        m_topLayout->setContentsMargins(0,0,0,0);
        m_topLayout->setAlignment(m_queryLineEdit,Qt::AlignCenter);
    }

}


/**
 * 进程开启监控槽函数
 */
void MainViewWidget::ViewOpenedSlot(QDBusMessage msg)
{
    QString path=msg.arguments().at(0).value<QString>();
    QString type=msg.arguments().at(1).value<QString>();
    if(QString::compare(type,"application")==0)
    {
        QDBusInterface ifaceapp("org.ayatana.bamf",path,
                                "org.ayatana.bamf.application",QDBusConnection::sessionBus());
        QDBusReply<QString> replyapp =ifaceapp.call("DesktopFile");
        QString desktopfp=replyapp.value();
        QStringList desktopfpList=m_ukuiMenuInterface->getDesktopFilePath();
        if(desktopfpList.contains(desktopfp))
        {
            QFileInfo fileInfo(desktopfp);
            QString desktopfn=fileInfo.fileName();

            QString dateTimeKey;
            dateTimeKey.clear();
            if(!desktopfn.isEmpty())
            {
                m_setting->beginGroup("lockapplication");
                bool ret=m_setting->contains(desktopfn);
                m_setting->endGroup();
                if(!ret)
                {
                    m_setting->beginGroup("application");
                    m_setting->setValue(desktopfn,m_setting->value(desktopfn).toInt()+1);
                    dateTimeKey=desktopfn;
                    m_setting->sync();
                    m_setting->endGroup();
                }
            }

            if(!dateTimeKey.isEmpty())
            {
                QDateTime dt=QDateTime::currentDateTime();
                int datetime=dt.toTime_t();
                m_setting->beginGroup("datetime");
                m_setting->setValue(dateTimeKey,datetime);
                m_setting->sync();
                m_setting->endGroup();
            }
        }
    }
}

void MainViewWidget::requestUpdateSlot()
{
    m_directoryChangedThread->quit();
    Q_EMIT directoryChangedSignal();
}

void MainViewWidget::iconThemeChangeSlot(QString key)
{
    if(key=="iconThemeName" || key=="icon-theme-name")
        Q_EMIT directoryChangedSignal();
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
    char style[100];
    sprintf(style, "QLineEdit{border:0px;background-color:%s;border-radius:2px;}",QueryLineEditBackground);
    m_queryLineEdit->setStyleSheet(style);
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
