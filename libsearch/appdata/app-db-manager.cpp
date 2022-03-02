#include "app-db-manager.h"
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QApplication>
using namespace UkuiSearch;
#define APP_DATABASE_PATH QDir::homePath()+"/.config/org.ukui/ukui-search/appdata/"+"app-info.db"
#define CONNECTION_NAME QLatin1String("ukss-appdb-connection")
static AppDBManager *global_instance = nullptr;
AppDBManager *AppDBManager::getInstance()
{
    if (!global_instance) {
        global_instance = new AppDBManager();
    }
    return global_instance;
}
AppDBManager::AppDBManager(QObject *parent) : QObject(parent), m_database(QSqlDatabase::addDatabase("QSQLITE",CONNECTION_NAME))
{
}

AppDBManager::~AppDBManager()
{

}
void AppDBManager::initDateBaseConnection()
{
    if(!m_database->isValid()) {
        qWarning() << m_database->lastError();
        QApplication::quit();
    }
    m_database->setDatabaseName(APP_DATABASE_PATH);
    if(!m_database->open()) {
        qWarning() << m_database->lastError();
        QApplication::quit();
    }
    //todo: 建表
}


