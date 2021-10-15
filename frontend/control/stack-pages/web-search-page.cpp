#include "web-search-page.h"
#include <QPainter>
#include "global-settings.h"
using namespace Zeeker;
WebSearchPage::WebSearchPage(QWidget *parent) : QWidget(parent)
{
    this->setFixedSize(720,56);
    initUI();
    initConnections();
}

//bool WebSearchPage::isSelected()
//{
//    return m_webSearchWidget->isSelected();
//}

void WebSearchPage::LaunchBrowser()
{
    this->m_webSearchWidget->LaunchBrowser();
}

void WebSearchPage::clearResultSelection()
{
    this->m_webSearchWidget->clearResultSelection();
}

void WebSearchPage::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(palette().base());
//    p.setOpacity(GlobalSettings::getInstance()->getValue(TRANSPARENCY_KEY).toDouble());
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(this->rect(), 16, 16);
}

void WebSearchPage::initUI()
{
//    m_mainLyt = new QVBoxLayout(this);
    m_webSearchWidget = new WebSearchWidget(this);
//    m_mainLyt->addWidget(m_webSearchWidget, Qt::AlignCenter);
    m_webSearchWidget->move(0, 0);
}

void WebSearchPage::initConnections()
{
    connect(this, &WebSearchPage::startSearch, m_webSearchWidget, &WebSearchWidget::startSearch);
    connect(this, &WebSearchPage::startSearch, this, [=] () {
        this->m_webSearchWidget->setResultSelection(this->m_webSearchWidget->getModlIndex(0, 0));//每次搜索默认选中websearch
    });
}

void WebSearchPage::setWebSearchSelection()
{
    this->m_webSearchWidget->setResultSelection(this->m_webSearchWidget->getModlIndex(0,0));
}
