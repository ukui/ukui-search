#ifndef UkuiSearchAppDataService_H
#define UkuiSearchAppDataService_H

#include <QObject>
#include <QCommandLineParser>
#include "qtsingleapplication.h"
namespace UkuiSearch {

class UkuiSearchAppDataService : public QtSingleApplication
{
    Q_OBJECT
public:
    UkuiSearchAppDataService(int &argc, char *argv[], const QString &applicationName = "ukui-search-app-data-service");

protected Q_SLOTS:
    void parseCmd(QString msg, bool isPrimary);
};
}
#endif // UkuiSearchAppDataService_H
