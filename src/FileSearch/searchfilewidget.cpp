#include "searchfilewidget.h"
#include <QApplication>
//#include <QPalette>
#include <QPalette>

SearchFileWidget::SearchFileWidget()
{
    initFilesearchUI();
    installEventFilter(this);

}

void SearchFileWidget::initFilesearchUI()
{
    //垂直布局
    m_listLayout = new QVBoxLayout(this);

    //添加表头
    m_fileHead = new HeadLabel(this);
    m_fileHead->setText(tr("File"));
    m_fileHead->setAttribute(Qt::WA_TranslucentBackground);//"透明背景"


    m_Morebutton=new MoreButton(this);
    m_Morebutton->setText("查看更多");

    //创建view与model
    fileView = new fileview;
    m_filemodel = new filemodel;
    fileView->setModel(m_filemodel);

    fileView->setColumnWidth(0,300);
    fileView->setColumnWidth(1,150);
    fileView->setColumnWidth(2,150);

    connect(m_Morebutton,&MoreButton::clicked,this,[=](){
        m_filemodel->run(5,5);
    });

    //创建文件搜索线程
    m_searchFileThread=new SearchFileThread;

    connect(this,&SearchFileWidget::sendSearchKeyword,
            m_searchFileThread,&SearchFileThread::recvSearchKeyword);

    connect(m_searchFileThread,&SearchFileThread::sendSearchResult,
            this,&SearchFileWidget::recvFileSearchResult);


    //监听点击事件，打开对应的文件
    connect(fileView,&QTreeView::clicked,this,[=](){
        m_filemodel->run(fileView->currentIndex().row(),fileView->currentIndex().column());
    });

    //监听输入框的改变，刷新界面
    QDBusConnection::sessionBus().connect(QString(), QString("/lineEdit/textChanged"), "org.ukui.search.inputbox", "InputBoxTextChanged", this, SLOT(fileTextRefresh(QString)));

    //整体布局
    m_listLayout->addWidget(m_fileHead);
    m_listLayout->addWidget(fileView);
    m_listLayout->addWidget(m_Morebutton);
    this->setLayout(m_listLayout);
    this->setVisible(false);

}

/**
 * @brief 监听信号刷新网页搜索的文字
 * @param mSearchText
 */
void SearchFileWidget::fileTextRefresh(QString mSearchText)
{
    m_filemodel->matchstart(mSearchText);
    if(!mSearchText.isEmpty()){
        m_searchFileThread->quit();
        m_searchFileThread->start();
        Q_EMIT sendSearchKeyword(mSearchText);
    }else{
        this->hide();
    }
}

void SearchFileWidget::recvFileSearchResult(QStringList arg)
{
    int count=m_filemodel->showResult(arg);
    m_searchFileThread->quit();
    if(count>2){
        this->setFixedHeight(m_fileHead->height()+(count+1.6)*46);
        m_Morebutton->show();
    }else{
        this->setFixedHeight(m_fileHead->height()+(count+1)*46);
        m_Morebutton->hide();
    }

    qDebug()<<"file list size :"<<arg.count();
    fileView->setCurrentIndex(m_filemodel->index(0,0,m_filemodel->index(0,0,m_filemodel->index(0))));
    //根据数据的大小隐藏或显示
    if(arg.count()>0) {
        this->setVisible(true);
    } else {
        this->setVisible(false);
        Q_EMIT fileWidgetHide();
    }
}

void SearchFileWidget::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    QRect rect = this->rect();
    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    p.setBrush(qApp->palette().color(QPalette::Base));
//    p.setBrush(QBrush(QColor(255,255,255)));
    p.setOpacity(1);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rect,12,12);
    QWidget::paintEvent(e);
}

bool SearchFileWidget::eventFilter(QObject *watched, QEvent *event){
    if(watched==this){
        if(event->type()==QEvent::MouseButtonRelease){
            return true;
        }
    }
    return false;
}

