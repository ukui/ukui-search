//
// Created by hxf on 22-6-7.
//

#include "icon-item.h"

#include <QUrl>
#include <QPainter>

using namespace UkuiSearch;

IconItem::IconItem()
{

}

void IconItem::paint(QPainter *painter)
{
    painter->drawPixmap(QRect(0, 0, width(), height()), m_icon.pixmap(width(), height()));
}

IconItem::~IconItem()
{

}

void IconItem::setIcon(const QIcon& icon)
{
    m_icon = icon;
    update();
}

QIcon IconItem::getIcon()
{
    return m_icon;
}

QString IconItem::getName()
{
    return m_icon.name();
}

void IconItem::setName(const QString &name)
{
    QIcon icon;
    if (name.isEmpty()) {
        if (m_backup.isEmpty()) {
            qDebug() << "IconItem: name is empty, backup is empty!";
            icon = QIcon(":/icons/desktop.png");

        } else {
            icon = QIcon::fromTheme(m_backup, QIcon("qrc:/icons/desktop.png"));
        }
    } else {
        icon = QIcon::fromTheme(name, QIcon(":/icons/desktop.png"));
    }
    //FIXME QIcon::fromTheme,主题框架，XDGIconLoader会出现找不到图标的情况
    setIcon(icon);
}

QString IconItem::getBackup()
{
    return m_backup;
}

void IconItem::setBackup(const QString &backup)
{
    m_backup = backup;
}
