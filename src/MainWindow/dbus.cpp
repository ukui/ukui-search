#include "dbus.h"
#include <QDebug>

DBus::DBus(QObject *parent) : QObject(parent)
{

}

/**
 * @brief DBus::GetApplicationDesktop 获取应用的desktop文件
 * @param arg  ：desktop文件
 * @return
 * 利用获取到的desktop文件可以用来解析图标和名称
 * 暂定可使用qtxdg进行解析
 */
bool DBus::GetApplicationDesktop(QString arg)
{
    qDebug()<<"GetApplicationDesktop is :"<<arg;
    return false;
}

/**
 * @brief DBus::GetSearchResult  获取搜索到的结果
 * @param arg 搜索结果
 * @return
 */
bool DBus::GetSearchResult(QStringList arg)
{
    qDebug()<<"GetSearchResult"<<arg;
    return false;
}

/**
 * @brief DBus::CheckIfExist 暂时不用此接口
 * @param arg
 * @return
 */
bool DBus::CheckIfExist(QString arg)
{
    return true;
}
