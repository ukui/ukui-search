#include "result-view-delegate.h"
#include <QPainterPath>
using namespace UkuiSearch;
static ResultItemStyle *global_instance_of_item_style = nullptr;

ResultViewDelegate::ResultViewDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

void ResultViewDelegate::setSearchKeyword(const QString &regFindKeyWords)
{
    m_regFindKeyWords.clear();
    m_regFindKeyWords = regFindKeyWords;
}

QSize ResultViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size =  QStyledItemDelegate::sizeHint(option,index);
    size.setHeight(size.height() + 10);
    return size;
}

void ResultViewDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const {
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    QStyle *style = opt.widget->style();

    QString text = opt.text;
    if(text.isEmpty()) {
        return;
    }
    opt.text = QString();
    style->proxy()->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget); //绘制非文本区域内容

    opt.text = text;
    QTextDocument doc;
    doc.setHtml(getHtmlText(painter, opt, index)); //提取富文本
    QAbstractTextDocumentLayout* layout = doc.documentLayout();
    const double height = layout->documentSize().height();


    QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &opt, opt.widget);
    //使图标和文本间隔与原来保持一致，故文本区域右移4
//    textRect.adjust(4, 0, 0, 0);
    double y = textRect.y();
    y += (textRect.height() - height) / 2;

    QAbstractTextDocumentLayout::PaintContext context;

    QPalette::ColorGroup cg = opt.state & QStyle::State_Enabled
            ? QPalette::Normal : QPalette::Disabled;
    if (cg == QPalette::Normal && !(opt.state & QStyle::State_Active))
        cg = QPalette::Inactive;

    if(opt.state & QStyle::State_Selected) {
        painter->setPen(opt.palette.color(cg, QPalette::HighlightedText));
    } else {
        painter->setPen(opt.palette.color(cg, QPalette::Text));
    }
    painter->save();
    painter->translate(QPointF(textRect.x(), y));
    layout->draw(painter, context); //绘制文本区域内容
    painter->restore();

}

QString ResultViewDelegate::getHtmlText(QPainter *painter, const QStyleOptionViewItem &itemOption, const QModelIndex &index) const
{
    int indexFindLeft = 0;
    QString indexString = index.model()->data(index, Qt::DisplayRole).toString();
    QFont ft(painter->font().family(), GlobalSettings::getInstance()->getValue(FONT_SIZE_KEY).toInt());
    QFontMetrics fm(ft);
    QString indexColString = fm.elidedText(indexString, Qt::ElideRight, itemOption.rect.width() - 30 - 10); //当字体超过Item的长度时显示为省略号
    QString htmlString;
    if((indexColString.toUpper()).contains((m_regFindKeyWords.toUpper()))) {
        indexFindLeft = indexColString.toUpper().indexOf(m_regFindKeyWords.toUpper()); //得到查找字体在当前整个Item字体中的位置
        htmlString = escapeHtml(indexColString.left(indexFindLeft)) + "<b>" + escapeHtml(indexColString.mid(indexFindLeft, m_regFindKeyWords.length())) + "</b>" + escapeHtml(indexColString.right(indexColString.length() - indexFindLeft - m_regFindKeyWords.length()));
    } else {
        bool boldOpenned = false;
        for(int i = 0; i < indexColString.length(); i++) {
            if((m_regFindKeyWords.toUpper()).contains(QString(indexColString.at(i)).toUpper())) {
                if(! boldOpenned) {
                    boldOpenned = true;
                    htmlString.append(QString("<b>"));
                }
                htmlString.append(escapeHtml(QString(indexColString.at(i))));
            } else {
                if(boldOpenned) {
                    boldOpenned = false;
                    htmlString.append(QString("</b>"));
                }
                htmlString.append(escapeHtml(QString(indexColString.at(i))));

            }
        }
    }
//    qDebug()<<indexColString<<"---->"<<htmlString;
    return "<pre>" + htmlString + "</pre>";
}

QString ResultViewDelegate::escapeHtml(const QString &str) const
{
    QString temp = str;
    temp.replace("<", "&lt;");
    temp.replace(">", "&gt;");
    return temp;
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
                path.addRoundedRect(option->rect, 6, 6);
            } else if(opt->viewItemPosition == QStyleOptionViewItem::Beginning) {
                //一个左侧有两个圆角的矩形
                path.moveTo(option->rect.topLeft() + QPoint(6, 0));
                path.cubicTo(option->rect.topLeft() + QPoint(6, 0),
                             option->rect.topLeft(), option->rect.topLeft() +
                             QPoint(0, 6));

                path.lineTo(option->rect.bottomLeft() - QPoint(0, 6));
                path.cubicTo(option->rect.bottomLeft() - QPoint(0, 6),
                             option->rect.bottomLeft() + QPoint(0, 1),
                             option->rect.bottomLeft() + QPoint(6, 1));

                path.lineTo(option->rect.bottomRight() + QPoint(1, 1));
                path.lineTo(option->rect.topRight()+ QPoint(1, 0));
                path.lineTo(option->rect.topLeft() + QPoint(6, 0));
            } else if(opt->viewItemPosition == QStyleOptionViewItem::Middle) {
                path.addRect(option->rect.adjusted(-1, 0, 1, 0));
            } else if(opt->viewItemPosition == QStyleOptionViewItem::End) {
                //一个右侧有两个圆角的矩形
                path.moveTo(option->rect.topRight() + QPoint(-6, 0));
                path.cubicTo(option->rect.topRight() + QPoint(-6, 0),
                             option->rect.topRight(),
                             option->rect.topRight() +
                             QPoint(0, 6));

                path.lineTo(option->rect.bottomRight() - QPoint(0, 6));
                path.cubicTo(option->rect.bottomRight() - QPoint(0, 6),
                             option->rect.bottomRight() + QPoint(0, 1),
                             option->rect.bottomRight() + QPoint(-6, 1));

                path.lineTo(option->rect.bottomLeft() + QPoint(0, 1));
                path.lineTo(option->rect.topLeft());
                path.lineTo(option->rect.topRight() + QPoint(-6, 0));
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
            break;
//            return QProxyStyle::drawPrimitive(element, option, painter, widget);
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
        break;
    }
}
