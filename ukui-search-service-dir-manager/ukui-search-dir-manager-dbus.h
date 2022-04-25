#ifndef UKUISEARCHDIRWATCHERDBUS_H
#define UKUISEARCHDIRWATCHERDBUS_H

#include "qtsingleapplication.h"

class UkuiSearchDirManagerDbus : public QtSingleApplication
{
    Q_OBJECT
public:
    UkuiSearchDirManagerDbus(int &argc, char *argv[], const QString &applicationName = "ukui-search-service-dir-manager");
protected Q_SLOTS:
    void parseCmd(QString msg, bool isPrimary);
};

#endif // UKUISEARCHDIRWATCHERDBUS_H
