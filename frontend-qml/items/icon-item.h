//
// Created by hxf on 22-6-7.
//

#ifndef UKUI_SEARCH_ICON_ITEM_H
#define UKUI_SEARCH_ICON_ITEM_H

#include <QQuickPaintedItem>
#include <QIcon>

namespace UkuiSearch {

class IconItem : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QIcon icon READ getIcon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(QString name READ getName WRITE setName)
    Q_PROPERTY(QString backup READ getBackup WRITE setBackup)
public:
    explicit IconItem();

    ~IconItem() override;

    void paint(QPainter *painter) override;

    QIcon getIcon();
    void setIcon(const QIcon& icon);

    QString getName();
    void setName(const QString& name);

    QString getBackup();
    void setBackup(const QString& backup);

Q_SIGNALS:
    void iconChanged();

private:
    QIcon m_icon;
    QString m_backup;
};

} // UkuiSearch

#endif //UKUI_SEARCH_ICON_ITEM_H
