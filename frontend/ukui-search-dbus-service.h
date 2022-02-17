#ifndef UKUISEARCHDBUSSERVICE_H
#define UKUISEARCHDBUSSERVICE_H

#include <QDBusConnection>
#include <QObject>

#include "mainwindow.h"

namespace Zeeker {

class UkuiSearchDbusServices: public QObject{
    Q_OBJECT

    Q_CLASSINFO("D-Bus Interface","org.ukui.search.service")

public:
    explicit UkuiSearchDbusServices(MainWindow *m);
    ~UkuiSearchDbusServices();

public Q_SLOTS:
    void showWindow();
    void hideWindow();
    void searchKeyword(QString keyword);

private:
    MainWindow *m_mainWindow;
};
}

#endif // UKUISEARCHDBUSSERVICE_H
