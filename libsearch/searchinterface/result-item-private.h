#ifndef RESULTITEMPRIVATE_H
#define RESULTITEMPRIVATE_H
#include "result-item.h"
#include "result-item-private.h"
namespace UkuiSearch {
class ResultItemPrivate
{
public:
    explicit ResultItemPrivate(ResultItem *parent);
    ~ResultItemPrivate();
    void setSearchId(size_t searchId);
    void setItemKey(QString itemKey);
    size_t getSearchId();
    QString getItemKey();
private:
    size_t m_searchId;
    QString m_itemKey;
    QString m_label;
    QVariant m_extral;
    //and something else...

    ResultItem *q;

};

}
#endif // RESULTITEMPRIVATE_H
