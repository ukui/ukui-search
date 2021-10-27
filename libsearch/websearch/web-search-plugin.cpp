#include "web-search-plugin.h"
#include "global-settings.h"

using namespace Zeeker;
WebSearchPlugin::WebSearchPlugin(QObject *parent) : QObject(parent)
{
    SearchPluginIface::Actioninfo open { 0, tr("Start browser search")};
    m_actionInfo << open;
    initDetailPage();
}

const QString Zeeker::WebSearchPlugin::name()
{
    return "Web Page";
}

const QString Zeeker::WebSearchPlugin::description()
{
    return tr("Web Page");
}

QString Zeeker::WebSearchPlugin::getPluginName()
{
    return tr("Web Page");
}

void Zeeker::WebSearchPlugin::KeywordSearch(QString keyword, DataQueue<Zeeker::SearchPluginIface::ResultInfo> *searchResult)
{
    m_keyWord = keyword;
    ResultInfo resultInfo;
    resultInfo.name = m_keyWord;
    resultInfo.type = 0;
    resultInfo.icon = QIcon(":/res/icons/search-web-icon.svg");
    resultInfo.actionKey = m_keyWord;
    searchResult->enqueue(resultInfo);
}

QList<Zeeker::SearchPluginIface::Actioninfo> Zeeker::WebSearchPlugin::getActioninfo(int type)
{
    return m_actionInfo;
}

void Zeeker::WebSearchPlugin::openAction(int actionkey, QString key, int type)
{
    QString address;
    QString engine = GlobalSettings::getInstance()->getValue("web_engine").toString();
    if(!engine.isEmpty()) {
        if(engine == "360") {
            address = "https://so.com/s?q=" + m_keyWord; //360
        } else if(engine == "sougou") {
            address = "https://www.sogou.com/web?query=" + m_keyWord; //搜狗
        } else {
            address = "http://baidu.com/s?word=" + m_keyWord; //百度
        }
    } else { //默认值
        address = "http://baidu.com/s?word=" + m_keyWord ; //百度
    }
    QDesktopServices::openUrl(address);
}

QWidget *Zeeker::WebSearchPlugin::detailPage(const Zeeker::SearchPluginIface::ResultInfo &ri)
{
    return m_detailPage;
}

void Zeeker::WebSearchPlugin::initDetailPage()
{
    m_detailPage = new QWidget();
    m_detailPage->setFixedWidth(360);
    m_detailPage->setAttribute(Qt::WA_TranslucentBackground);
    m_detailLyt = new QVBoxLayout(m_detailPage);
    m_detailLyt->setContentsMargins(8, 0, 16, 0);
    m_iconLabel = new QLabel(m_detailPage);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    QString type = GlobalSettings::getInstance()->getValue(STYLE_NAME_KEY).toString();
    if (type == "ukui-dark") {
        m_iconLabel->setPixmap(QIcon(":/res/icons/search-web-dark.svg").pixmap(100, 96));
    } else {
        m_iconLabel->setPixmap(QIcon(":/res/icons/search-web-default.svg").pixmap(100, 96));
    }
    connect(qApp, &QApplication::paletteChanged, this, [=] () {
        QString type = GlobalSettings::getInstance()->getValue(STYLE_NAME_KEY).toString();
        if (type == "ukui-dark") {
            m_iconLabel->setPixmap(QIcon(":/res/icons/search-web-dark.svg").pixmap(100, 96));
        } else {
            m_iconLabel->setPixmap(QIcon(":/res/icons/search-web-default.svg").pixmap(100, 96));
        }
    });

    m_actionFrame = new QFrame(m_detailPage);
    m_actionFrameLyt = new QVBoxLayout(m_actionFrame);
    m_actionFrameLyt->setContentsMargins(0, 0, 0, 0);
    m_actionFrameLyt->setAlignment(Qt::AlignCenter);
    m_actionLabel1 = new ActionLabel(tr("Start browser search"), m_currentActionKey, m_actionFrame);
    m_actionLabel1->adjustSize();

    m_actionFrameLyt->addWidget(m_actionLabel1);;
    m_actionFrame->setLayout(m_actionFrameLyt);

    m_detailLyt->addSpacing(166);
    m_detailLyt->addWidget(m_iconLabel);
    m_detailLyt->addSpacing(6);
    m_detailLyt->addWidget(m_actionFrame);
    m_detailPage->setLayout(m_detailLyt);
    m_detailLyt->addStretch();

    connect(m_actionLabel1, &ActionLabel::actionTriggered, [ & ](){
        openAction(0, m_currentActionKey, 0);
    });
}
