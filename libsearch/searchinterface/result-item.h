#ifndef RESULTITEM_H
#define RESULTITEM_H

#include <QString>
#include <QVariant>
namespace UkuiSearch {
class ResultItemPrivate;
class ResultItem
{
public:
    explicit ResultItem(const QString &itemKey);
    ~ResultItem();
private:
    ResultItemPrivate *d;
};
}

#endif // RESULTITEM_H
