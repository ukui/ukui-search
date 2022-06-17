#include "result-view-delegate.h"
using namespace Zeeker;
static ResultItemStyle *global_instance_of_item_style = nullptr;

ResultViewDelegate::ResultViewDelegate(QObject *parent) : QStyledItemDelegate(parent),
    m_textDoc(new QTextDocument(this)),
    m_hightLightEffectHelper(new HightLightEffectHelper(this))
{
}

void ResultViewDelegate::setSearchKeyword(const QString &regFindKeyWords)
{
    m_hightLightEffectHelper->setExpression(regFindKeyWords);
}

QSize ResultViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size =  QStyledItemDelegate::sizeHint(option,index);
//    size.setHeight(size.height() + 10);
    return size;
}

void ResultViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    opt.displayAlignment = Qt::Alignment(Qt::AlignLeft|Qt::AlignVCenter);

    QString text = opt.text;
    opt.text = QString();

    QStyle *style = opt.widget->style();
    style->proxy()->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget); //绘制非文本区域内容

    opt.text = text;
    QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &opt, opt.widget);
    painter->save();
    if(opt.state & QStyle::State_Selected) {
        m_hightLightEffectHelper->setTextColor(QBrush(opt.palette.highlightedText().color()));
    } else {
        m_hightLightEffectHelper->setTextColor(QBrush(opt.palette.text().color()));
    }
    painter->translate(textRect.topLeft());

    m_textDoc->setHtml("<pre>" + text + "</pre>");
    m_hightLightEffectHelper->setDocument(m_textDoc);
    m_hightLightEffectHelper->rehighlight();
    m_textDoc->drawContents(painter);
    painter->restore();
}

ResultItemStyle *ResultItemStyle::getStyle()
{
    if (!global_instance_of_item_style) {
        global_instance_of_item_style = new ResultItemStyle;
    }
    return global_instance_of_item_style;
}

void ResultItemStyle::drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    switch (element) {
    case PE_PanelItemViewItem: {

        bool isIconView = false;
        auto opt = qstyleoption_cast<const QStyleOptionViewItem *>(option);
        if (!opt)
            return;
        if (opt) {
            isIconView = (opt->decorationPosition & QStyleOptionViewItem::Top);
        }

        bool isHover = (option->state & State_MouseOver) && (option->state & ~State_Selected);
        bool isSelected = option->state & State_Selected;
        bool enable = option->state & State_Enabled;
        QColor color = option->palette.color(enable? QPalette::Active: QPalette::Disabled, QPalette::Highlight);

        if (isSelected) {
            color.setAlpha(255);
        } else if (isHover) {
//            color = opt->palette.color(QPalette::Active, QPalette::BrightText);
//            color.setAlpha(0.05);
            color = QColor(241, 241, 241);
        } else {
            color.setAlpha(0);
        }
        QPainterPath path;

        if(opt->viewItemPosition == QStyleOptionViewItem::OnlyOne) {
            path.addRoundedRect(option->rect, 8, 8);
        } else if(opt->viewItemPosition == QStyleOptionViewItem::Beginning) {
            //一个左侧有两个圆角的矩形
            path.moveTo(option->rect.topLeft() + QPoint(8, 0));
            path.cubicTo(option->rect.topLeft() + QPoint(8, 0),
                         option->rect.topLeft(), option->rect.topLeft() +
                         QPoint(0, 8));

            path.lineTo(option->rect.bottomLeft() - QPoint(0, 8));
            path.cubicTo(option->rect.bottomLeft() - QPoint(0, 8),
                         option->rect.bottomLeft() + QPoint(0, 1),
                         option->rect.bottomLeft() + QPoint(8, 1));

            path.lineTo(option->rect.bottomRight() + QPoint(1, 1));
            path.lineTo(option->rect.topRight()+ QPoint(1, 0));
            path.lineTo(option->rect.topLeft() + QPoint(8, 0));
        } else if(opt->viewItemPosition == QStyleOptionViewItem::Middle) {
            path.addRect(option->rect.adjusted(-1, 0, 1, 0));
        } else if(opt->viewItemPosition == QStyleOptionViewItem::End) {
            //一个右侧有两个圆角的矩形
            path.moveTo(option->rect.topRight() + QPoint(-8, 0));
            path.cubicTo(option->rect.topRight() + QPoint(-8, 0),
                         option->rect.topRight(),
                         option->rect.topRight() +
                         QPoint(0, 8));

            path.lineTo(option->rect.bottomRight() - QPoint(0, 8));
            path.cubicTo(option->rect.bottomRight() - QPoint(0, 8),
                         option->rect.bottomRight() + QPoint(0, 1),
                         option->rect.bottomRight() + QPoint(-8, 1));

            path.lineTo(option->rect.bottomLeft() + QPoint(0, 1));
            path.lineTo(option->rect.topLeft());
            path.lineTo(option->rect.topRight() + QPoint(-8, 0));
        } else {
            //                    path.addRoundedRect(option->rect, 8, 8);
        }

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(Qt::NoPen);
        painter->setBrush(color);
        painter->drawPath(path);
        //            painter->fillPath(path, painter->brush());

        painter->restore();
        break;
    }
    default:
        return QProxyStyle::drawPrimitive(element, option, painter, widget);
    }
}

void ResultItemStyle::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    switch (element) {
    case CE_ItemViewItem: {
        if (const QStyleOptionViewItem *vopt = qstyleoption_cast<const QStyleOptionViewItem *>(option)) {
            painter->save();
            if (painter->clipPath().isEmpty()) {
                painter->setClipRect(option->rect);
            }

            QRect checkRect = proxy()->subElementRect(SE_ItemViewItemCheckIndicator, vopt, widget);
            QRect iconRect = proxy()->subElementRect(SE_ItemViewItemDecoration, vopt, widget);

            // draw the background
            proxy()->drawPrimitive(PE_PanelItemViewItem, option, painter, widget);

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
                default:
                    break;
                }
                proxy()->drawPrimitive(QStyle::PE_IndicatorViewItemCheck, &option, painter, widget);
            }

            // draw the icon
            QIcon::Mode mode = QIcon::Normal;
            if (!(vopt->state & QStyle::State_Enabled)) {
                mode = QIcon::Disabled;
            } else if (vopt->state & QStyle::State_Selected) {
                mode = QIcon::Selected;
            }
            QIcon::State state = vopt->state & QStyle::State_Open ? QIcon::On : QIcon::Off;
            auto pixmap = vopt->icon.pixmap(vopt->decorationSize, mode, state);

            iconRect.moveLeft(8);
            QStyle::drawItemPixmap(painter, iconRect, vopt->decorationAlignment, pixmap);
            painter->restore();
            return;
        }
        break;
    }
    default:
        return QProxyStyle::drawControl(element, option, painter, widget);
    }
}

HightLightEffectHelper::HightLightEffectHelper(QObject *parent) : QSyntaxHighlighter(parent)
{
    m_expression.setCaseSensitivity(Qt::CaseInsensitive);
}

void HightLightEffectHelper::setExpression(const QString &text)
{
    m_expression.setPattern(text);
}

void HightLightEffectHelper::setTextColor(const QBrush &brush)
{
    m_textCharFormat.setForeground(brush);
}

void HightLightEffectHelper::highlightBlock(const QString &text)
{
    setFormat(0, text.length(), m_textCharFormat);

    m_textCharFormat.setFontWeight(QFont::Bold);
    int index = text.indexOf(m_expression);
    while(index >= 0){
        int length = m_expression.matchedLength();
        setFormat(index, length, m_textCharFormat);
        index = text.indexOf(m_expression, index+length);
    }
    m_textCharFormat.setFontWeight(QFont::Normal);
}
