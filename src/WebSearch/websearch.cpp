#include "websearch.h"
#include <QPalette>
#include <QApplication>

websearch::websearch()
{
    this->setStyleSheet("QPushButton{text-align: left;}");
    this->setFixedHeight(40);
    QColor color1 = palette().color(QPalette::Base);
    QPalette pal(this->palette());
    pal.setColor(QPalette::Button, QColor(color1));
    this->setPalette(pal);
    this->setVisible(false);
    searchString = "请使用百度搜索：";
    search = "";


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
        this->setText(searchString);
    } else {
        search=mSearchText;
        this->setVisible(true);
        this->setText(searchString);
    }

}

//void websearch::paintEvent(QPaintEvent *e)
//{
//    QPainter p(this);
//    QRect rect = this->rect();
//    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
//    p.setOpacity(1);
//    p.setPen(Qt::NoPen);
//    p.drawRoundedRect(rect,12,12);
//    QWidget::paintEvent(e);
//}

