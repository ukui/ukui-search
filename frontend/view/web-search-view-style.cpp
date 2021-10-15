#include "web-search-view-style.h"
#include <QStyleOption>
#include <QPainterPath>
#include <QPainter>
#include <QDebug>
using namespace Zeeker;
static WebSearchViewStyle *global_instance = nullptr;
WebSearchViewStyle::WebSearchViewStyle(QObject *parent) : QProxyStyle()
{
}

WebSearchViewStyle *WebSearchViewStyle::getStyle()
{
    if (!global_instance)
        global_instance = new WebSearchViewStyle;
    return global_instance;
}

void WebSearchViewStyle::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    // text will not be drawn here as it will be drawn in delegate.
    if (element == CE_ItemViewItem) {
        auto p = painter;
        auto opt = option;
        if (const QStyleOptionViewItem *vopt = qstyleoption_cast<const QStyleOptionViewItem *>(opt)) {
            p->save();
            if (p->clipPath().isEmpty())
                p->setClipRect(opt->rect);

            QRect checkRect = proxy()->subElementRect(SE_ItemViewItemCheckIndicator, vopt, widget);
            QRect iconRect = proxy()->subElementRect(SE_ItemViewItemDecoration, vopt, widget);

            // draw the background
            this->drawPrimitive(PE_PanelItemViewItem, opt, p, widget);

            // draw the check mark
            if (vopt->features & QStyleOptionViewItem::HasCheckIndicator) {
                QStyleOptionViewItem option(*vopt);
                option.rect = checkRect;
                option.state = option.state & ~QStyle::State_HasFocus;

                switch (vopt->checkState) {
                case Qt::Unchecked:
                    option.state |= QStyle::State_Off;
                    break;
                case Qt::PartiallyChecked:
                    option.state |= QStyle::State_NoChange;
                    break;
                case Qt::Checked:
                    option.state |= QStyle::State_On;
                    break;
                }
                drawPrimitive(QStyle::PE_IndicatorViewItemCheck, &option, p, widget);
            }

            // draw the icon
            QIcon::Mode mode = QIcon::Normal;
            if (!(vopt->state & QStyle::State_Enabled))
                mode = QIcon::Disabled;
            else if (vopt->state & QStyle::State_Selected)
                mode = QIcon::Selected;
            QIcon::State state = vopt->state & QStyle::State_Open ? QIcon::On : QIcon::Off;
            //vopt->icon.paint(p, iconRect, vopt->decorationAlignment, mode, state);
            auto pixmap = vopt->icon.pixmap(vopt->decorationSize, mode, state);
            auto target = pixmap;
            drawItemPixmap(painter, iconRect, vopt->decorationAlignment, target);
            p->restore();
            return;
        }
    }
    return QProxyStyle::drawControl(element, option, painter, widget);
}

void WebSearchViewStyle::drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if (element == PE_PanelItemViewItem) {
        auto opt = qstyleoption_cast<const QStyleOptionViewItem *>(option);
        if (!opt)
            return;
        bool alternate=opt->features & QStyleOptionViewItem::Alternate;
        bool isHover = (option->state & State_MouseOver) && (option->state & ~State_Selected);
        bool isSelected = option->state & State_Selected;
        bool enable = option->state & State_Enabled;

        QColor color = option->palette.color(enable? QPalette::Active: QPalette::Disabled,
                                             QPalette::Highlight);
        QColor color2 = option->palette.color(enable? QPalette::Active: QPalette::Disabled,
                                              QPalette::Button);
        color.setAlpha(0);
        color2.setAlpha(0);

        if (alternate)
        {
            if(isHover | isSelected){
                color = option->palette.highlight().color();
            }
            else{
                color = option->palette.alternateBase().color();
                color.setAlpha(255);
            }
        }

        if (isHover && !isSelected) {
            int h = color.hsvHue();
            auto base = option->palette.base().color();
            int v = color.value();
            color.setHsv(h, base.lightness(), v, 64);
            color2.setAlpha(100);
        }
        if (isSelected) {
            color.setAlpha(255);
            color2.setAlpha(255);
        }

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(Qt::transparent);
        painter->setBrush(color);
        painter->drawRoundedRect(option->rect, 16, 16);
        painter->restore();
        return;
    }
    return QProxyStyle::drawPrimitive(element, option, painter, widget);
}
