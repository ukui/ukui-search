#ifndef RESULTITEM_H
#define RESULTITEM_H

#include <QString>
#include <QVariant>
namespace UkuiSearch {
class ResultItemPrivate;
class ResultItem
{
public:
    explicit ResultItem();
    virtual ~ResultItem();

    ResultItem(const ResultItem &item);
    explicit ResultItem(const size_t searchId);
    explicit ResultItem(const QString itemKey);
    explicit ResultItem(const size_t searchId, const QString itemKey, QVariantList extral = QVariantList());
    size_t getSearchId() const;
    QString getItemKey() const;
    QVariantList getExtral() const;

private:
    ResultItemPrivate *d;
};
}

#endif // RESULTITEM_H
