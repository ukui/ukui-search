#include "result-item.h"
#include "result-item-private.h"
using namespace UkuiSearch;
ResultItemPrivate::ResultItemPrivate::ResultItemPrivate(const QString &itemKey, ResultItem *parent)
{

}

ResultItemPrivate::~ResultItemPrivate()
{

}
ResultItem::ResultItem(const QString &itemKey)
{
    d = new ResultItemPrivate(itemKey, this);
}

ResultItem::~ResultItem()
{
}
