#ifndef HOMEPAGESECTION_H
#define HOMEPAGESECTION_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QEvent>
#include <QDebug>
#include <QPainter>
#include <QApplication>
#include <QStyleOption>
#include <math.h>
#include "flow-layout/flow-layout.h"

namespace Zeeker {

enum class HomePageItemShape {
    Square = 0,
    Bar
};
struct HomePageItem
{
    QIcon icon;
    QString name;
    QString key;
    QString action;
    QString pluginId;
};

class ItemWidget : public QWidget {
    Q_OBJECT
public:
    explicit ItemWidget(QWidget *, const HomePageItemShape &, const HomePageItem &);
    ~ItemWidget() = default;

protected:
    bool eventFilter(QObject *, QEvent *);
    void paintEvent(QPaintEvent *);

private:
    void initUi(HomePageItemShape, const QString&, const QIcon&);

    QHBoxLayout * m_hlayout = nullptr;
    QVBoxLayout * m_vlayout = nullptr;
    QLabel * m_iconlabel = nullptr;
    QLabel * m_namelabel = nullptr;
    double m_transparency = 0;
    HomePageItem m_item;

Q_SIGNALS:
    void clicked(const QString &key, const QString &action, const QString &pluginId);
};

class HomePageSection : public QWidget
{
    Q_OBJECT
public:
    explicit HomePageSection(QString title, HomePageItemShape shape = HomePageItemShape::Square, QWidget *parent = nullptr);
    ~HomePageSection();

    void setItems(QVector<HomePageItem> itemList);
    int length();
//    void appendItem(HomePageItem item);
//    void insertItem(const int &index, const HomePageItem &item);
//    void removeOne(const QString &key);
    void clear();

Q_SIGNALS:
    //emit on HomePageItem clicked
    void requestAction(const QString &key, const QString &action, const QString &pluginId);

private:
    QString m_title;
    QVector<HomePageItem> m_items;
    void initUi();
//    void resize();
    QVBoxLayout * m_mainLyt = nullptr;
    QLabel * m_titleLabel = nullptr;
    QWidget *m_itemWidget = nullptr;
    FlowLayout * m_itemsLyt = nullptr;
    HomePageItemShape m_shape;
    int m_length;

    ItemWidget* createSquareItem(const HomePageItem &item);
    ItemWidget* createBarItem(const HomePageItem &item);
};
}

#endif // HOMEPAGESECTION_H
