#include "result-item.h"
#include "result-item-private.h"
using namespace UkuiSearch;
ResultItem::ResultItem(const QString &itemKey)
{
    d = new ResultItemPrivate(itemKey, this);
}

ResultItem::~ResultItem()
{
}
