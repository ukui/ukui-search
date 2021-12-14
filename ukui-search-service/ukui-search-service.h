#ifndef UKUISEARCHSERVICE_H
#define UKUISEARCHSERVICE_H

#include <QObject>
#include <QCommandLineParser>
#include <QGSettings/QGSettings>
#include "qtsingleapplication.h"
#include "search-method-manager.h"
#include "common.h"
#include "file-utils.h"
namespace UkuiSearch {

class UkuiSearchService : public QtSingleApplication
{
    Q_OBJECT
public:
    UkuiSearchService(int &argc, char *argv[], const QString &applicationName = "ukui-search-service");

protected Q_SLOTS:
    void parseCmd(QString msg, bool isPrimary);
private:
    void initGsettings();
    void setSearchMethodByGsettings();
    void indexServiceSwitch(bool startIndex = true);
    QGSettings *m_SearchGsettings;
};
}
#endif // UKUISEARCHSERVICE_H
