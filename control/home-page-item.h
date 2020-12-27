#ifndef HOMEPAGEITEM_H
#define HOMEPAGEITEM_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "file-utils.h"
#include "search-list-view.h"

class HomePageItem : public QWidget
{
    Q_OBJECT
public:
    explicit HomePageItem(QWidget *, const int&, const QString&);
    ~HomePageItem();

    enum ItemType { //homepage中item的类型，包括常用应用、最近打开、快捷打开
        Common,
        Recent,
        Quick
    };

private:
    void setupUi(const int&, const QString&);

    QWidget * m_widget = nullptr;
    QHBoxLayout * m_hlayout = nullptr;
    QVBoxLayout * m_vlayout = nullptr;
    QLabel * m_iconlabel = nullptr;
    QLabel * m_namelabel = nullptr;

Q_SIGNALS:

};

#endif // HOMEPAGEITEM_H
