//
// Created by hxf on 22-6-7.
//

#include "icon-item.h"

#include <utility>

using namespace UkuiSearch;

IconItem::IconItem()
{

}

void IconItem::paint(QPainter *painter)
{
    m_icon.paint(painter, QRect(0, 0, width(), height()));
}

IconItem::~IconItem()
{

}

void IconItem::setIcon(const QIcon& icon)
{
    m_icon = icon;
}

QIcon IconItem::getIcon()
{
    return QIcon();
}

QString IconItem::getName()
{
    return m_icon.name();
}

void IconItem::setName(const QString &name)
{
    if (name.isEmpty()) {
        if (m_backup.isEmpty()) {
            qDebug() << "IconItem: name is empty, backup is empty!";
            m_icon = QIcon(":/icons/desktop.png");

        } else {
            m_icon = QIcon::fromTheme(m_backup, QIcon("qrc:/icons/desktop.png"));
        }
        return;
    }
    //FIXME QIcon::fromTheme,主题框架，XDGIconLoader会出现找不到图标的情况
    m_icon = QIcon::fromTheme(name, QIcon(":/icons/desktop.png"));
}

QString IconItem::getBackup()
{
    return m_backup;
}

void IconItem::setBackup(const QString &backup)
{
    m_backup = backup;
}
