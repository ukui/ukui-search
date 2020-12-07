#include "dbus.h"
#include <QDebug>

DBus::DBus(QObject *parent) : QObject(parent)
{

}

bool DBus::GetApplicationDesktop(QString arg)
{
    qDebug()<<"GetApplicationDesktop is :"<<arg;
    return false;
}

bool DBus::GetSearchResult(QStringList arg)
{
    qDebug()<<"GetSearchResult"<<arg;
    return false;
}
bool DBus::CheckIfExist(QString arg)
{
    return true;
}
