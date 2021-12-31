#ifndef RESULTITEMPRIVATE_H
#define RESULTITEMPRIVATE_H
#include "result-item.h"
#include "result-item-private.h"
namespace UkuiSearch {
class ResultItemPrivate
{
public:
    explicit ResultItemPrivate(const QString &itemKey, ResultItem *parent);
    ~ResultItemPrivate();
private:
    size_t searchId;
    QString m_itemKey;
    QString m_label;
    QVariant m_extral;
    //and something else...

    ResultItem *q;

};
}
#endif // RESULTITEMPRIVATE_H
