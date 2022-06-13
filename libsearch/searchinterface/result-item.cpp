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

void ResultItemPrivate::setExtral(QVariantList extral)
{
    for (auto &info : extral) {
        m_extral.append(info);
    }
}

size_t ResultItemPrivate::getSearchId()
{
    return m_searchId;
}

QString ResultItemPrivate::getItemKey()
{
    return m_itemKey;
}

QVariantList ResultItemPrivate::getExtral()
{
    return m_extral;
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

ResultItem::ResultItem(const size_t searchId, const QString itemKey, QVariantList extral) : d(new ResultItemPrivate(this))
{
    d->setSearchId(searchId);
    d->setItemKey(itemKey);
    d->setExtral(extral);
}


size_t ResultItem::getSearchId() const
{
    return d->getSearchId();
}

QString ResultItem::getItemKey() const
{
    return d->getItemKey();
}

QVariantList ResultItem::getExtral() const
{
    return d->getExtral();
}

ResultItem::~ResultItem()
{
    if (d)
        delete d;
    d = nullptr;
}

ResultItem::ResultItem(const ResultItem &item): d(new ResultItemPrivate(this))
{
    d->setSearchId(item.getSearchId());
    d->setItemKey(item.getItemKey());
    d->setExtral(item.getExtral());
}
