#ifndef WEBSEARCHVIEWDELEGATE_H
#define WEBSEARCHVIEWDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QStyle>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
namespace Zeeker {
class WebSearchViewDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    WebSearchViewDelegate(QObject *parent);
    ~WebSearchViewDelegate() = default;
    void setSearchKeyword(const QString &);
protected:
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
    QString m_regFindKeyWords = 0;
    void paint(QPainter *, const QStyleOptionViewItem &, const QModelIndex &) const override;
    QString escapeHtml(const QString&) const;
};
}

#endif // WEBSEARCHVIEWDELEGATE_H
