#include "app-info-table.h"
#include "app-info-table-private.h"
using namespace UkuiSearch;
AppInfoTablePrivate::AppInfoTablePrivate(AppInfoTable *parent) : QObject(parent), q(parent)
{
}

AppInfoTable::AppInfoTable(QObject *parent) : QObject(parent), d(new AppInfoTablePrivate(this))
{
}
