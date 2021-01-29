#include "highlight-item-delegate.h"
#include <QPainter>
#include <QApplication>
#include <QStyle>
#include <QDebug>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

HighlightItemDelegate::HighlightItemDelegate()
{
}

/**
 * \brief treewidget_styledItemDelegate::paint
 * \param painter
 * \param option
 * \param index
 */
void HighlightItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    QStyleOptionViewItemV4 optionV4 = option;
    initStyleOption(&optionV4, index);

    QStyle *style = optionV4.widget? optionV4.widget->style() : QApplication::style();

    optionV4.text = QString();
    style->drawControl(QStyle::CE_ItemViewItem, &optionV4, painter); //绘制非文本区域内容
    if (index.model()->data(index,Qt::DisplayRole).toString().isEmpty()) return;

    QTextDocument doc;
    doc.setHtml(getHtmlText(painter, option, index)); //提取富文本
    QAbstractTextDocumentLayout::PaintContext ctx;

    if (optionV4.state & QStyle::State_Selected)
        ctx.palette.setColor(QPalette::Text, optionV4.palette.color(QPalette::Active, QPalette::HighlightedText));

    QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &optionV4);
    textRect.adjust(-20, -5, 0, 0);
    painter->save();
    painter->translate(textRect.topLeft());
    painter->setClipRect(textRect.translated(-textRect.topLeft()));
    doc.documentLayout()->draw(painter, ctx); //绘制文本区域内容
    painter->restore();

    //以下为drawitemtext方法绘制文字，印加粗字体与原字体不等宽导致像素不对齐，视觉效果较差，已暂时摒弃
//    QStyleOptionViewItem itemOption = option;
//    initStyleOption(&itemOption, index);
//    if (itemOption.state & QStyle::State_HasFocus) {
//       itemOption.state = itemOption.state ^ QStyle::State_HasFocus;//去掉选中时候的Item的虚线框，也就是焦点
//    }
//    QStyledItemDelegate::paint(painter, itemOption, index);
//    findKeyWord(painter, itemOption, index);
}

/**
 * @brief HighlightItemDelegate::getHtmlText 将文本区域内容转换为标记语言形式（在关键字左右加<b></b>来加粗部分文字）
 * @param painter
 * @param itemOption
 * @param index
 * @return
 */
QString HighlightItemDelegate::getHtmlText(QPainter *painter, const QStyleOptionViewItem &itemOption, const QModelIndex &index) const
{
    int indexFindLeft = 0;
    QString indexString = index.model()->data(index,Qt::DisplayRole).toString();
    QFontMetrics m_QFontMetrics = painter->fontMetrics();
    QString indexColString = m_QFontMetrics.elidedText(indexString, Qt::ElideRight, itemOption.rect.width() + 10); //当字体超过Item的长度时显示为省略号
    QString htmlString;
    if ((indexColString.toUpper()).contains((m_regFindKeyWords.toUpper()))) {
        indexFindLeft = indexColString.toUpper().indexOf(m_regFindKeyWords.toUpper()); //得到查找字体在当前整个Item字体中的位置
//        paintKeywordHighlight(painter, itemOption, indexColString, indexFindLeft, m_regFindKeyWords.length());
        htmlString = escapeHtml(indexColString.left(indexFindLeft)) + "<b>" + escapeHtml(indexColString.mid(indexFindLeft, m_regFindKeyWords.length())) + "</b>" + escapeHtml(indexColString.right(indexColString.length() - indexFindLeft - m_regFindKeyWords.length()));
    } else {
        bool boldOpenned = false;
        for (int i = 0; i < indexColString.length(); i++) {
            if ((m_regFindKeyWords.toUpper()).contains(QString(indexColString.at(i)).toUpper())) {
//                paintKeywordHighlight(painter, itemOption, indexColString, i, 1);
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
    return htmlString;
}

/**
 * @brief HighlightItemDelegate::escapeHtml 将文件名原本带的尖括号转义，以防被识别为富文本
 * @param str
 * @return
 */
QString HighlightItemDelegate::escapeHtml(const QString & str) const
{
    QString temp = str;
    temp.replace("<", "&lt;");
    temp.replace(">", "&gt;");
    return temp;
}

/**
 * @brief HighlightItemDelegate::paintKeywordHighlight drawitemtext方法绘制文字，印加粗字体与原字体不等宽导致像素不对齐，视觉效果较差，已暂时摒弃，保留以做底色高亮等备用
 * @param painter
 * @param itemOption
 * @param indexColString 完整字符串
 * @param indexFindLeft 关键字位置
 * @param keywordLength 关键字长度
 */
void HighlightItemDelegate::paintKeywordHighlight(QPainter *painter, const QStyleOptionViewItem &itemOption, const QString &indexColString, const int &indexFindLeft, const int &keywordLength) const
{
    QPen pen(Qt::black);
    painter->setPen(pen);
    QFont font = QApplication::font(itemOption.widget);
//    QFont boldFont = QApplication::font(itemOption.widget);
//    boldFont.setBold(true);
//    boldFont.setStyleStrategy(QFont::PreferAntialias);
    painter->setFont(font);
    painter->setRenderHint(QPainter::Antialiasing);

    QFontMetrics m_leftFontMetrics = painter->fontMetrics();//得到这个item原字体的像素
//    painter->setFont(boldFont);
//    QFontMetrics m_QFontMetrics = painter->fontMetrics();//得到这个加粗字体的像素
    const QWidget * m_QWidget = itemOption.widget;
    QStyle * m_QStyle = m_QWidget ? m_QWidget->style() : QApplication::style(); //得到当前的style
    QRect m_QRect = itemOption.rect;//得到Item的自己的Rect
    QPalette::ColorRole textDisplayRole = QPalette::NoRole; //设置text的role
    if (itemOption.state & QStyle::State_Selected)
    {
        textDisplayRole = QPalette::HighlightedText; //当选中字体的时候字体显示高亮
    } else {
        textDisplayRole = QPalette::Text;
    }

    int findKeyWordWidth = m_leftFontMetrics.width(m_regFindKeyWords); //得到查找字体的像素宽度
    int preFindKeyWordWidth = m_leftFontMetrics.width(indexColString.mid(0,indexFindLeft)); //得到查找字体前面的字体的像素宽度

    m_QRect = m_QRect.adjusted(preFindKeyWordWidth + 3, 0, findKeyWordWidth, 0);
    //高亮字段
    m_QStyle->drawItemText(painter, m_QRect, itemOption.displayAlignment, QApplication::palette(), true, indexColString.mid(indexFindLeft, keywordLength), textDisplayRole);
//    painter->restore();
}

/**
 * \brief treewidget_styledItemDelegate::search_keyword 赋值关键字
 * \param regFindKeyWords
 */
void HighlightItemDelegate::setSearchKeyword(const QString &regFindKeyWords)
{
    m_regFindKeyWords.clear();
    m_regFindKeyWords = regFindKeyWords;
}
