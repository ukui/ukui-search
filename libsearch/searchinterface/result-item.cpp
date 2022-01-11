#include "result-item.h"
#include "result-item-private.h"
using namespace UkuiSearch;
ResultItemPrivate::ResultItemPrivate::ResultItemPrivate(ResultItem *parent) : q(parent)
{
}

ResultItemPrivate::~ResultItemPrivate()
{

}

void ResultItemPrivate::setSearchId(size_t searchId)
{
    m_searchId = searchId;
}

void ResultItemPrivate::setItemKey(QString itemKey)
{
    m_itemKey = itemKey;
}
ResultItem::ResultItem() : d(new ResultItemPrivate(this))
{
}

ResultItem::ResultItem(const size_t searchId) : d(new ResultItemPrivate(this))
{
    d->setSearchId(searchId);
}

ResultItem::ResultItem(const QString itemKey) : d(new ResultItemPrivate(this))
{
    d->setItemKey(itemKey);
}

ResultItem::ResultItem(const size_t searchId, const QString itemKey)
{
    d->setSearchId(searchId);
    d->setItemKey(itemKey);
}

ResultItem::~ResultItem()
{
}
