#include "web-search-view-delegate.h"
#include "web-search-view.h"
#include <QApplication>
using namespace Zeeker;
WebSearchViewDelegate::WebSearchViewDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}
void WebSearchViewDelegate::setSearchKeyword(const QString &regFindKeyWords)
{
    m_regFindKeyWords.clear();
    m_regFindKeyWords = regFindKeyWords;
}

QSize WebSearchViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size =  QStyledItemDelegate::sizeHint(option,index);
    size.setHeight(56);
    return size;
}

void WebSearchViewDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    opt.displayAlignment = Qt::Alignment(Qt::AlignLeft|Qt::AlignVCenter);
    WebSearchView *view = qobject_cast<WebSearchView *>(parent());

    view->style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter);
    auto rect = view->visualRect(index);

    //绘制文本区域内容
    QString newText = qApp->fontMetrics().elidedText(m_regFindKeyWords, Qt::ElideRight, 480);
    QTextDocument doc;
    doc.setHtml(tr("Search '<b>%1</b>' with Baidu.").arg(escapeHtml(newText)));
    QAbstractTextDocumentLayout::PaintContext ctx;

    if(opt.state & QStyle::State_Selected)
        ctx.palette.setColor(QPalette::Text, opt.palette.color(QPalette::Active, QPalette::HighlightedText));
    QRect textRect = view->style()->subElementRect(QStyle::SE_ItemViewItemText, &opt);
    painter->save();
    painter->translate(textRect.adjusted(16, 12, -16, -12).topLeft());
    doc.documentLayout()->draw(painter, ctx);
    painter->restore();

    //绘制右侧箭头
    QIcon icon = QIcon::fromTheme("go-next-symbolic");
    icon.paint(painter, rect.right() - 40, rect.height()/2-8,  16, 16, Qt::AlignCenter);
}

QString WebSearchViewDelegate::escapeHtml(const QString &str) const
{
    QString temp = str;
    temp.replace("<", "&lt;");
    temp.replace(">", "&gt;");
    return temp;
}
