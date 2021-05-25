#include "result-view-delegate.h"
using namespace Zeeker;

ResultViewDelegate::ResultViewDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

void ResultViewDelegate::setSearchKeyword(const QString &regFindKeyWords)
{
    m_regFindKeyWords.clear();
    m_regFindKeyWords = regFindKeyWords;
}

void ResultViewDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const {
    QStyleOptionViewItemV4 optionV4 = option;
    initStyleOption(&optionV4, index);

    QStyle *style = optionV4.widget ? optionV4.widget->style() : QApplication::style();

    optionV4.text = QString();
    style->drawControl(QStyle::CE_ItemViewItem, &optionV4, painter); //绘制非文本区域内容
    if(index.model()->data(index, Qt::DisplayRole).toString().isEmpty()) return;

    QTextDocument doc;
    doc.setHtml(getHtmlText(painter, option, index)); //提取富文本
    QAbstractTextDocumentLayout::PaintContext ctx;

    if(optionV4.state & QStyle::State_Selected)
        ctx.palette.setColor(QPalette::Text, optionV4.palette.color(QPalette::Active, QPalette::HighlightedText));

    QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &optionV4);
    textRect.adjust(0, -5, 0, 0);
    painter->save();
    painter->translate(textRect.topLeft());
    painter->setClipRect(textRect.translated(-textRect.topLeft()));
    doc.documentLayout()->draw(painter, ctx); //绘制文本区域内容
    painter->restore();
}

QString ResultViewDelegate::getHtmlText(QPainter *painter, const QStyleOptionViewItem &itemOption, const QModelIndex &index) const
{
    int indexFindLeft = 0;
    QString indexString = index.model()->data(index, Qt::DisplayRole).toString();
    QFont ft(painter->font().family(), GlobalSettings::getInstance()->getValue(FONT_SIZE_KEY).toInt());
    QFontMetrics fm(ft);
    QString indexColString = fm.elidedText(indexString, Qt::ElideRight, itemOption.rect.width() - 30); //当字体超过Item的长度时显示为省略号
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
    return htmlString;
}

QString ResultViewDelegate::escapeHtml(const QString &str) const
{
    QString temp = str;
    temp.replace("<", "&lt;");
    temp.replace(">", "&gt;");
    return temp;
}
