#ifndef HIGHLIGHTITEMDELEGATE_H
#define HIGHLIGHTITEMDELEGATE_H

#include <QStyledItemDelegate>

class HighlightItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit HighlightItemDelegate();
    void setSearchKeyword(const QString &);
private:
    QString m_regFindKeyWords = 0;
    void paint(QPainter *,const QStyleOptionViewItem &, const QModelIndex &) const override;
    QString getHtmlText(QPainter *, const QStyleOptionViewItem &, const QModelIndex &) const;
    QString escapeHtml(const QString&) const;
    void paintKeywordHighlight(QPainter *, const QStyleOptionViewItem &, const QString &, const int &, const int &) const;
};

#endif // HIGHLIGHTITEMDELEGATE_H
