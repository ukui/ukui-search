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

size_t ResultItemPrivate::getSearchId()
{
    return m_searchId;
}

QString ResultItemPrivate::getItemKey()
{
    return m_itemKey;
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

ResultItem::ResultItem(const size_t searchId, const QString itemKey) : d(new ResultItemPrivate(this))
{
    d->setSearchId(searchId);
    d->setItemKey(itemKey);
}

size_t ResultItem::getSearchId()
{
    return d->getSearchId();
}

QString ResultItem::getItemKey()
{
    return d->getItemKey();
}

ResultItem::~ResultItem()
{
}