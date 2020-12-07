#ifndef DBUS_H
#define DBUS_H

#include <QObject>

class DBus : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface","org.ukui.search.searchresult")
public:
    explicit DBus(QObject *parent = 0);

public Q_SLOTS:
    bool GetApplicationDesktop(QString arg);
    bool GetSearchResult(QStringList arg);
    bool CheckIfExist(QString arg);
};

#endif // DBUS_H
