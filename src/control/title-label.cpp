#include "title-label.h"
#include <QPainter>
#include <QStyleOption>

TitleLabel::TitleLabel(QWidget * parent) : QLabel(parent)
{
    this->setContentsMargins(8, 0, 0, 0);
    this->setFixedHeight(24);
}

TitleLabel::~TitleLabel()
{

}

void TitleLabel::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event)

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QRect rect = this->rect();
    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    p.setBrush(opt.palette.color(QPalette::Text));
    p.setOpacity(0.06);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rect, 0, 0);
    return QLabel::paintEvent(event);
}
