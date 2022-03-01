#ifndef APPINFOTABLEPRIVATE_H
#define APPINFOTABLEPRIVATE_H
#include <QObject>
#include <app-info-table.h>
namespace UkuiSearch {
class AppInfoTablePrivate : public QObject
{
    Q_OBJECT
public:
    explicit AppInfoTablePrivate(AppInfoTable *parent = nullptr);
    AppInfoTablePrivate(AppInfoTablePrivate &) = delete;
    AppInfoTablePrivate &operator =(const AppInfoTablePrivate &) = delete;

private:
    AppInfoTable *q;

};


}
#endif // APPINFOTABLEPRIVATE_H
