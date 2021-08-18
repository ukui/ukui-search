#include "web-search-label.h"
#include <QIcon>
#include <QDesktopServices>
#include "global-settings.h"
using namespace Zeeker;
WebSearchLabel::WebSearchLabel(QWidget *parent) : QLabel(parent)
{
    initUi();
    this->installEventFilter(this);
}

void WebSearchLabel::initUi()
{
    this->setFixedHeight(30);
    this->setContentsMargins(8, 0, 0, 0);
    this->setFixedWidth(656);

    m_WebSearchIconlabel = new QLabel(this);
    m_WebSearchIconlabel->setFixedHeight(30);
    m_WebSearchIconlabel->setPixmap(QIcon::fromTheme("edit-find-symbolic").pixmap(QSize(16, 16)));

    m_WebSearchLabel = new QLabel(this);
    m_WebSearchLabel->setFixedHeight(30);

    m_webSearchLyt = new QHBoxLayout(this);
    m_webSearchLyt->setContentsMargins(0, 0, 0, 0);
    m_webSearchLyt->addWidget(m_WebSearchIconlabel);
    m_webSearchLyt->addWidget(m_WebSearchLabel);
    m_webSearchLyt->addStretch();
    this->setLayout(m_webSearchLyt);
    m_defultStyleSheet = this->styleSheet();
}

void WebSearchLabel::startSearch()
{
    //新打开网页搜索或关键词发生变化，重新load
    QString keyword = m_WebSearchLabel->text();//目前网页搜索的关键词，记录此词来判断网页是否需要刷新
    QString address;
    QString engine = GlobalSettings::getInstance()->getValue("web_engine").toString();
    if(!engine.isEmpty()) {
        if(engine == "360") {
            address = "https://so.com/s?q=" + keyword; //360
        } else if(engine == "sougou") {
            address = "https://www.sogou.com/web?query=" + keyword; //搜狗
        } else {
            address = "http://baidu.com/s?word=" + keyword; //百度
        }
    } else { //默认值
        address = "http://baidu.com/s?word=" + keyword; //百度
    }
    QDesktopServices::openUrl(address);

}

QString WebSearchLabel::getDefultStyleSheet()
{
    return m_defultStyleSheet;
}

bool WebSearchLabel::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == this) {
        if(event->type() == QEvent::MouseButtonPress) {
            this->setStyleSheet("background-color: #3790FA");//#3790FA选中颜色，
            return true;
        } else if(event->type() == QEvent::MouseButtonRelease) {
            startSearch();
            return true;
        } else if(event->type() == QEvent::Enter) {
            this->setStyleSheet("background-color: #87CEFA");//TODO鼠标悬浮颜色待定
            return true;
        } else if(event->type() == QEvent::Leave) {
            this->setStyleSheet(m_defultStyleSheet);//默认颜色
            return true;
        }
    }
    return QObject::eventFilter(watched, event);
}

void WebSearchLabel::initConnections()
{

}

void WebSearchLabel::webSearch(const QString &key)
{
    m_WebSearchLabel->setText(key);
}
