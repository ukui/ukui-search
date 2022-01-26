#ifndef UKUISEARCHGUI_H
#define UKUISEARCHGUI_H

#include <QObject>

#include "qtsingleapplication.h"
#include "mainwindow.h"
#include "ukui-search-dbus-service.h"
#undef Bool
namespace UkuiSearch {
class UkuiSearchGui : public QtSingleApplication
{
    Q_OBJECT
public:
    UkuiSearchGui(int &argc, char *argv[], const QString &applicationName = "ukui-search-gui");
    ~UkuiSearchGui();

protected Q_SLOTS:
    void parseCmd(QString msg, bool isPrimary);

private:
    MainWindow *m_mainWindow = nullptr;
    UkuiSearchDbusServices *m_dbusService = nullptr;
};
}

#endif // UKUISEARCHGUI_H
