#include "result-view-delegate.h"
using namespace Zeeker;
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
//    size.setHeight(size.height() + 10);
    return size;
}

void ResultViewDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const {
    QStyleOptionViewItemV4 optionV4 = option;
    initStyleOption(&optionV4, index);

    QStyle *style = optionV4.widget ? optionV4.widget->style() : QApplication::style();

    optionV4.text = QString();
    style->drawControl(QStyle::CE_ItemViewItem, &optionV4, painter); //绘制非文本区域内容
    if (index.model()->data(index, Qt::DisplayRole).toString().isEmpty()) return;

    //fix me: for files which name begin with some ' ' , space will be hide...
    QTextDocument doc;
    doc.setHtml(getHtmlText(painter, option, index)); //提取富文本
    QAbstractTextDocumentLayout::PaintContext ctx;

    if (optionV4.state & QStyle::State_Selected)
        ctx.palette.setColor(QPalette::Text, optionV4.palette.color(QPalette::Active, QPalette::HighlightedText));

    QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &optionV4);
    //使图标和文本间隔与原来保持一致，故文本区域右移4
    textRect.adjust(4, 0, 0, 0);
    painter->save();
    painter->translate(QPointF(textRect.x(), textRect.y() + 6));
//    painter->translate(textRect.topLeft());
//    painter->setClipRect(textRect.translated(-textRect.topLeft()));
    doc.documentLayout()->draw(painter, ctx); //绘制文本区域内容
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
    if ((indexColString.toUpper()).contains((m_regFindKeyWords.toUpper()))) {
        indexFindLeft = indexColString.toUpper().indexOf(m_regFindKeyWords.toUpper()); //得到查找字体在当前整个Item字体中的位置
        htmlString = escapeHtml(indexColString.left(indexFindLeft)) + "<b>" + escapeHtml(indexColString.mid(indexFindLeft, m_regFindKeyWords.length())) + "</b>" + escapeHtml(indexColString.right(indexColString.length() - indexFindLeft - m_regFindKeyWords.length()));
    } else {
        bool boldOpenned = false;
        for(int i = 0; i < indexColString.length(); i++) {
            if ((m_regFindKeyWords.toUpper()).contains(QString(indexColString.at(i)).toUpper())) {
                if (! boldOpenned) {
                    boldOpenned = true;
                    htmlString.append(QString("<b>"));
                }
                htmlString.append(escapeHtml(QString(indexColString.at(i))));
            } else {
                if (boldOpenned) {
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
