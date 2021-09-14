#include "web-search-page.h"
#include <QPainter>
#include "global-settings.h"
using namespace Zeeker;
WebSearchPage::WebSearchPage(QWidget *parent) : QWidget(parent)
{
    this->setFixedSize(720,56);
}

void WebSearchPage::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(palette().base());
    p.setOpacity(GlobalSettings::getInstance()->getValue(TRANSPARENCY_KEY).toDouble());
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(this->rect(), 16, 16);
}
