#include "websearch.h"
#include <QPalette>
#include <QApplication>

webwidget::webwidget(){
    this->setVisible(false);
    this->setFixedHeight(50);
    websearch *web=new websearch(this);
    QHBoxLayout *buttonLayout=new QHBoxLayout(this);
    buttonLayout->addWidget(web);
    QDBusConnection::sessionBus().connect(QString(), QString("/lineEdit/textChanged"), "org.ukui.search.inputbox", "InputBoxTextChanged", this,SLOT(webSearchTextRefresh(QString)));

}

void webwidget::webSearchTextRefresh(QString mSearchText){
    if(mSearchText == ""){
        this->setVisible(false);
    } else {
        this->setVisible(true);
    }
}

void webwidget::paintEvent(QPaintEvent *e){
    QPainter p(this);
    QRect rect = this->rect();
    QColor color1 = palette().color(QPalette::Base);
    QBrush brush =QBrush(color1);
    p.setBrush(brush);
    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rect,12,12);

    QWidget::paintEvent(e);
}

websearch::websearch(QWidget *parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setParent(parent);
    this->setFixedHeight(35);
    this->setVisible(false);
    searchString = "请使用百度搜索：";
    search = "";
    QColor color1 = palette().color(QPalette::Base);
    color1.setAlpha(0);
    QPalette pal(this->palette());
    pal.setColor(QPalette::Button, QColor(color1));
    this->setPalette(pal);
    m_buttonLayout=new QHBoxLayout(this);
    m_buttonText=new QLabel(this);
    m_Iconlabel=new QLabel(this);
    QPixmap pixmap(":/data/img/sidebarwidget/searching-more.svg");
    m_Iconlabel->setPixmap(pixmap);
    m_buttonLayout->addSpacing(5);
    m_buttonLayout->addWidget(m_buttonText);
    m_buttonLayout->addWidget(m_Iconlabel,0,Qt::AlignRight);
    m_buttonLayout->addSpacing(1);


    //监听输入框的改变，刷新界面
    QDBusConnection::sessionBus().connect(QString(), QString("/lineEdit/textChanged"), "org.ukui.search.inputbox", "InputBoxTextChanged", this, SLOT(webSearchTextRefresh(QString)));

    connect(this,&QPushButton::clicked,this,[=](){
           QString str = QString::fromLocal8Bit("https://www.baidu.com/baidu?tn=ubuntuu_cb&ie=utf-8&wd=").append(search);
           QProcess p;
           p.setProgram(QString::fromLocal8Bit("chromium-browser"));
           p.setArguments(QStringList()<<str);
           p.startDetached(p.program(), p.arguments());
           p.waitForFinished(-1);
       });

}

/**
 * @brief 监听信号刷新网页搜索的文字
 * @param mSearchText
 */
void websearch::webSearchTextRefresh(QString mSearchText)
{
    searchString=QString::fromLocal8Bit("   使用百度搜索").append(QString::fromLocal8Bit(" ")).append(QString::fromLocal8Bit("\"")).append(mSearchText).append(QString::fromLocal8Bit("\""));
    if(mSearchText == ""){
        search=mSearchText;
        this->setVisible(false);
        m_buttonText->setText(searchString);
    } else {
        search=mSearchText;
        this->setVisible(true);
        m_buttonText->setText(searchString);
    }

}

