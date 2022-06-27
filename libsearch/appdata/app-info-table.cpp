#include <QDebug>
#include <QtGlobal>
#include <string>
#include <QStandardPaths>
#include <QProcess>
#include <QDBusInterface>
#include <QDBusReply>
#include <QFileInfo>
#include <QTime>
#include <QSqlQuery>
#include <QSqlError>
#include <QApplication>
#include <qt5xdg/XdgDesktopFile>
#include "app-info-table.h"
#include "app-info-table-private.h"
#include "../ukui-search-app-data-service/app-db-common-defines.h"

using namespace UkuiSearch;
AppInfoTablePrivate::AppInfoTablePrivate(AppInfoTable *parent) : QObject(parent), q(parent), m_database(new QSqlDatabase)
{
    while(1) {
        srand(QTime(0,0,0).secsTo(QTime::currentTime()));
        m_ConnectionName = QString::fromStdString(std::to_string(rand()));//随机生产链接
        if (!QSqlDatabase::contains(m_ConnectionName))
            break;
    }
    qDebug() << "App info database currunt connection name:" << m_ConnectionName;
    if (!this->openDataBase()) {
        Q_EMIT q->DBOpenFailed();
        qWarning() << "Fail to open App DataBase, because:" << m_database->lastError();
    }
}

bool AppInfoTablePrivate::setAppFavoritesState(QString &desktopfp, size_t num)
{
    bool res(true);
    if (m_database->transaction()) {
        QSqlQuery sql(*m_database);
        QString cmd = QString("UPDATE appInfo SET MODIFYED_TIME='%0', FAVORITES=%1 WHERE DESKTOP_FILE_PATH='%2'")
                .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                .arg(num)
                .arg(desktopfp);
        if (!sql.exec(cmd)) {
            qWarning() << "Set app favorites state failed!" << m_database->lastError();
            res = false;
        }
        if (!m_database->commit()) {
            qWarning() << "Failed to commit !" << cmd;
            m_database->rollback();
            res = false;
        }
    } else {
        qWarning() << "Failed to start transaction mode!!!";
        res = false;
    }
    return res;
}

bool AppInfoTablePrivate::setAppTopState(QString &desktopfp, size_t num)
{
    bool res(true);
    if (m_database->transaction()) {
        QSqlQuery sql(*m_database);
        QString cmd = QString("UPDATE appInfo SET MODIFYED_TIME='%0', TOP=%1 WHERE DESKTOP_FILE_PATH='%2'")
                .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                .arg(num)
                .arg(desktopfp);
        if (!sql.exec(cmd)) {
            qWarning() << "Set app favorites state failed!" << m_database->lastError();
            res = false;
        }
        if (!m_database->commit()) {
            qWarning() << "Failed to commit !" << cmd;
            m_database->rollback();
            res = false;
        }
    } else {
        qWarning() << "Failed to start transaction mode!!!";
        res = false;
    }
    return res;
}

bool AppInfoTablePrivate::setAppLaunchTimes(QString &desktopfp, size_t num)
{
    bool res(true);
    if (m_database->transaction()) {
        QSqlQuery sql(*m_database);
        QString cmd = QString("UPDATE appInfo SET MODIFYED_TIME='%0', LAUNCH_TIMES=%1, LAUNCHED=%2 WHERE DESKTOP_FILE_PATH='%3'")
                .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                .arg(num)
                .arg(1)
                .arg(desktopfp);
        if (!sql.exec(cmd)) {
            qWarning() << "Set app favorites state failed!" << m_database->lastError();
            res = false;
        }
        if (!m_database->commit()) {
            qWarning() << "Failed to commit !" << cmd;
            m_database->rollback();
            res = false;
        }
    } else {
        qWarning() << "Failed to start transaction mode!!!";
        res = false;
    }
    return res;
}

bool AppInfoTablePrivate::updateAppLaunchTimes(QString &desktopfp)
{
    bool res(true);
    if (m_database->transaction()) {
        QSqlQuery sql(*m_database);
        QString cmd = QString("SELECT LAUNCH_TIMES FROM appInfo WHERE DESKTOP_FILE_PATH='%1'").arg(desktopfp);
        if (sql.exec(cmd)) {
            if (sql.next()) {
                cmd = QString("UPDATE appInfo SET MODIFYED_TIME='%0', LAUNCH_TIMES=%1, LAUNCHED=%2 WHERE DESKTOP_FILE_PATH='%3'")
                        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                        .arg(sql.value(0).toInt() + 1)
                        .arg(1)
                        .arg(desktopfp);
                if (!sql.exec(cmd)) {
                    qWarning() << "Set app favorites state failed!" << m_database->lastError();
                    res = false;
                }
            } else {
                qWarning() << "Failed to exec next!" << cmd;
                res = false;
            }
        } else {
            qWarning() << "Failed to exec:" << cmd;
            res = false;
        }
        if (!m_database->commit()) {
            qWarning() << "Failed to commit !" << cmd;
            m_database->rollback();
            res = false;
        }
    } else {
        qWarning() << "Failed to start transaction mode!!!";
        res = false;
    }
    return res;
}

bool AppInfoTablePrivate::setAppLockState(QString &desktopfp, size_t num)
{
    bool res(true);
    if (m_database->transaction()) {
        QSqlQuery sql(*m_database);
        QString cmd = QString("UPDATE appInfo SET MODIFYED_TIME='%0', LOCK=%1 WHERE DESKTOP_FILE_PATH='%2'")
                .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                .arg(num)
                .arg(desktopfp);
        if (!sql.exec(cmd)) {
            qWarning() << "Set app favorites state failed!" << m_database->lastError();
            res = false;
        }
        if (!m_database->commit()) {
            qWarning() << "Failed to commit !" << cmd;
            m_database->rollback();
            res = false;
        }
    } else {
        qWarning() << "Failed to start transaction mode!!!";
        res = false;
    }
    return res;
}

bool AppInfoTablePrivate::getAllAppDesktopList(QStringList &list)
{
    bool res(true);
    if (m_database->transaction()) {
        QSqlQuery sql(*m_database);
        QString cmd = QString("SELECT DESKTOP_FILE_PATH FROM appInfo");
        if (sql.exec(cmd)) {
            while (sql.next()) {
                list.append(sql.value(0).toString());
            }
        } else {
            qWarning() << QString("cmd %0 failed!").arg(cmd) << m_database->lastError();
            res = false;
        }
        if (!m_database->commit()) {
            qWarning() << "Failed to commit !" << cmd;
            m_database->rollback();
            res = false;
        }
    } else {
        qWarning() << "Failed to start transaction mode!!!";
        res = false;
    }
    return res;
}

bool AppInfoTablePrivate::getFavoritesAppList(QStringList &list)
{
    bool res(true);
    if (m_database->transaction()) {
        QSqlQuery sql(*m_database);
        QSqlQuery sqlque(*m_database);
        QString cmd = QString("SELECT DESKTOP_FILE_PATH FROM appInfo WHERE FAVORITES!=0 ORDER BY FAVORITES");
        int count = 0;
        if (sql.exec(cmd)) {
            while (sql.next()) {
                list.append(sql.value(0).toString());
                cmd = QString("UPDATE appInfo SET FAVORITES=%1 WHERE DESKTOP_FILE_PATH='%2'")
                        .arg(++count)
                        .arg(sql.value(0).toString());
                if (!sqlque.exec(cmd)) {
                    qWarning() << QString("cmd %0 failed!").arg(cmd) << m_database->lastError();
                    res = false;
                    break;
                }
            }
        } else {
            qWarning() << QString("cmd %0 failed!").arg(cmd) << m_database->lastError();
            res = false;
        }
        if (!m_database->commit()) {
            qWarning() << "Failed to commit !" << cmd;
            m_database->rollback();
            res = false;
        }
    } else {
        qWarning() << "Failed to start transaction mode!!!";
        res = false;
    }
    return res;
}

bool AppInfoTablePrivate::getTopAppList(QStringList &list)
{
    bool res(true);
    if (m_database->transaction()) {
        QSqlQuery sql(*m_database);
        QSqlQuery sqlque(*m_database);
        QString cmd = QString("SELECT DESKTOP_FILE_PATH FROM appInfo WHERE TOP!=0 ORDER BY TOP");
        int count = 0;
        if (sql.exec(cmd)) {
            while (sql.next()) {
                list.append(sql.value(0).toString());
                cmd = QString("UPDATE appInfo SET TOP=%1 WHERE DESKTOP_FILE_PATH='%2'")
                        .arg(++count)
                        .arg(sql.value(0).toString());
                if (!sqlque.exec(cmd)) {
                    qWarning() << QString("cmd %0 failed!").arg(cmd) << m_database->lastError();
                    res = false;
                    break;
                }
            }
        } else {
            qWarning() << QString("cmd %0 failed!").arg(cmd) << m_database->lastError();
            res = false;
        }
        if (!m_database->commit()) {
            qWarning() << "Failed to commit !" << cmd;
            m_database->rollback();
            res = false;
        }
    } else {
        qWarning() << "Failed to start transaction mode!!!";
        res = false;
    }
    return res;
}

bool AppInfoTablePrivate::getLaunchTimesAppList(QStringList &list)
{
    bool res(true);
    if (m_database->transaction()) {
        QSqlQuery sql(*m_database);
        QSqlQuery sqlque(*m_database);
        QString cmd = QString("SELECT DESKTOP_FILE_PATH FROM appInfo ORDER BY LAUNCH_TIMES");
        int count = 0;
        if (sql.exec(cmd)) {
            while (sql.next()) {
                list.append(sql.value(0).toString());
                cmd = QString("UPDATE appInfo SET TOP=%1 WHERE DESKTOP_FILE_PATH='%2'")
                        .arg(++count)
                        .arg(sql.value(0).toString());
                if (!sqlque.exec(cmd)) {
                    qWarning() << QString("cmd %0 failed!").arg(cmd) << m_database->lastError();
                    res = false;
                    break;
                }
            }
        } else {
            qWarning() << QString("cmd %0 failed!").arg(cmd) << m_database->lastError();
            res = false;
        }
        if (!m_database->commit()) {
            qWarning() << "Failed to commit !" << cmd;
            m_database->rollback();
            res = false;
        }
    } else {
        qWarning() << "Failed to start transaction mode!!!";
        res = false;
    }
    return res;
}

bool AppInfoTablePrivate::getAppCategory(QString &desktopfp, QString &category)
{
    bool res(true);
    if (m_database->transaction()) {
        QSqlQuery sql(*m_database);
        QString cmd = QString("SELECT CATEGORY FROM appInfo WHERE DESKTOP_FILE_PATH='%0'")
                .arg(desktopfp);
        if (sql.exec(cmd)) {
            if (sql.next()) {
                category = sql.value(0).toString();
            } else {
                qWarning() << QString("cmd %0 failed!").arg(cmd) << m_database->lastError();
                res = false;
            }
        } else {
            qWarning() << QString("cmd %0 failed!").arg(cmd) << m_database->lastError();
            res = false;
        }
        if (!m_database->commit()) {
            qWarning() << "Failed to commit !" << cmd;
            m_database->rollback();
            res = false;
        }
    } else {
        qWarning() << "Failed to start transaction mode!!!";
        res = false;
    }
    return res;
}

bool AppInfoTablePrivate::getAppInfoResults(QVector<AppInfoTable::AppInfoResult> &appInfoResults)
{
    bool res(true);
    if (m_database->transaction()) {
        QSqlQuery sql(*m_database);
        QString cmd = QString("SELECT DESKTOP_FILE_PATH,LOCAL_NAME,ICON,CATEGORY,TOP,FAVORITES,LAUNCH_TIMES,LOCK FROM appInfo");
        if (sql.exec(cmd)) {
            while (sql.next()) {
                AppInfoTable::AppInfoResult result;
                result.desktopPath = sql.value(0).toString();
                result.appLocalName = sql.value(1).toString();
                result.iconName = sql.value(2).toString();
                result.category = sql.value(3).toString();
                result.top = sql.value(4).toInt();
                result.favorate = sql.value(5).toInt();
                result.launchTimes = sql.value(6).toInt();
                result.lock = sql.value(7).toInt();
                appInfoResults.append(std::move(result));
            }
        } else {
            qWarning() << QString("cmd %0 failed!").arg(cmd) << m_database->lastError();
            res = false;
        }
        if (!m_database->commit()) {
            qWarning() << "Failed to commit !" << cmd;
            m_database->rollback();
            res = false;
        }
    } else {
        qWarning() << "Failed to start transaction mode!!!";
        res = false;
    }
    return res;
}

bool AppInfoTablePrivate::getAppLockState(QString &desktopfp, size_t &num)
{
    bool res(true);
    if (m_database->transaction()) {
        QSqlQuery sql(*m_database);
        QString cmd = QString("SELECT LOCK FROM appInfo WHERE DESKTOP_FILE_PATH='%0'")
                .arg(desktopfp);
        if (sql.exec(cmd)) {
            if (sql.next()) {
                num = sql.value(0).toInt();
            } else {
                qWarning() << QString("cmd %0 failed!").arg(cmd) << m_database->lastError();
                res = false;
            }
        } else {
            qWarning() << QString("cmd %0 failed!").arg(cmd) << m_database->lastError();
            res = false;
        }
        if (!m_database->commit()) {
            qWarning() << "Failed to commit !" << cmd;
            m_database->rollback();
            res = false;
        }
    } else {
        qWarning() << "Failed to start transaction mode!!!";
        res = false;
    }
    return res;
}

bool AppInfoTablePrivate::getAppTopState(QString &desktopfp, size_t &num)
{
    bool res(true);
    if (m_database->transaction()) {
        QSqlQuery sql(*m_database);
        QString cmd = QString("SELECT TOP FROM appInfo WHERE DESKTOP_FILE_PATH='%0'")
                .arg(desktopfp);
        if (sql.exec(cmd)) {
            if (sql.next()) {
                num = sql.value(0).toInt();
            } else {
                qWarning() << QString("cmd %0 failed!").arg(cmd) << m_database->lastError();
                res = false;
            }
        } else {
            qWarning() << QString("cmd %0 failed!").arg(cmd) << m_database->lastError();
            res = false;
        }
        if (!m_database->commit()) {
            qWarning() << "Failed to commit !" << cmd;
            m_database->rollback();
            res = false;
        }
    } else {
        qWarning() << "Failed to start transaction mode!!!";
        res = false;
    }
    return res;
}

bool AppInfoTablePrivate::getAppLaunchedState(QString &desktopfp, size_t &num)
{
    bool res(true);
    if (m_database->transaction()) {
        QSqlQuery sql(*m_database);
        QString cmd = QString("SELECT LAUNCHED FROM appInfo WHERE DESKTOP_FILE_PATH='%0'")
                .arg(desktopfp);
        if (sql.exec(cmd)) {
            if (sql.next()) {
                num = sql.value(0).toInt();
            } else {
                qWarning() << QString("cmd %0 failed!").arg(cmd) << m_database->lastError();
                res = false;
            }
        } else {
            qWarning() << QString("cmd %0 failed!").arg(cmd) << m_database->lastError();
            res = false;
        }
        if (!m_database->commit()) {
            qWarning() << "Failed to commit !" << cmd;
            m_database->rollback();
            res = false;
        }
    } else {
        qWarning() << "Failed to start transaction mode!!!";
        res = false;
    }
    return res;
}

bool AppInfoTablePrivate::getAppFavoriteState(QString &desktopfp, size_t &num)
{
    bool res(true);
    if (m_database->transaction()) {
        QSqlQuery sql(*m_database);
        QString cmd = QString("SELECT FAVORITES FROM appInfo WHERE DESKTOP_FILE_PATH='%0'")
                .arg(desktopfp);
        if (sql.exec(cmd)) {
            if (sql.next()) {
                num = sql.value(0).toInt();
            } else {
                qWarning() << QString("cmd %0 failed!").arg(cmd) << m_database->lastError();
                res = false;
            }
        } else {
            qWarning() << QString("cmd %0 failed!").arg(cmd) << m_database->lastError();
            res = false;
        }
        if (!m_database->commit()) {
            qWarning() << "Failed to commit !" << cmd;
            m_database->rollback();
            res = false;
        }
    } else {
        qWarning() << "Failed to start transaction mode!!!";
        res = false;
    }
    return res;
}

bool AppInfoTablePrivate::addAppShortcut2Desktop(QString &desktopfp)
{
    bool res(true);
    QString dirpath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QFileInfo fileInfo(desktopfp);
    QString desktopfn = fileInfo.fileName();
    QFile file(desktopfp);
    QString newName = QString(dirpath + "/" + desktopfn);
    if(file.copy(QString(dirpath + "/" + desktopfn))) {
        QProcess process;
        process.startDetached(QString("chmod a+x %1").arg(newName));
    } else {
        res = false;
    }
    return res;
}

bool AppInfoTablePrivate::addAppShortcut2Panel(QString &desktopfp)
{
    bool res(true);
    QDBusInterface iface("com.ukui.panel.desktop",
                         "/",
                         "com.ukui.panel.desktop",
                         QDBusConnection::sessionBus());
    if(iface.isValid()) {
        QDBusReply<bool> isExist = iface.call("CheckIfExist", desktopfp);
        if(isExist) {
            qWarning() << "Add shortcut to panel failed, because it is already existed!";
        } else {
            QDBusReply<bool> ret = iface.call("AddToTaskbar", desktopfp);
            if (ret.value()) {
                qDebug() << "Add shortcut to panel success.";
            } else {
                qWarning() << "Add shortcut to panel failed, reply:" << ret.error();
                res = false;
            }
        }
    } else {
        res = false;
    }
    return res;
}

bool AppInfoTablePrivate::searchInstallApp(QString &keyWord, QStringList &installAppInfoRes)
{
    bool res(true);
    if (m_database->transaction()) {
        QSqlQuery sql(*m_database);
        QString cmd;
        if (keyWord.size() < 2) {
            cmd = QString("SELECT DESKTOP_FILE_PATH,LOCAL_NAME,ICON FROM appInfo WHERE LOCAL_NAME OR NAME_EN OR NAME_ZH OR FIRST_LETTER_OF_PINYIN LIKE '%%0%' ORDER BY FAVORITES DESC")
                    .arg(keyWord);
        } else {
            cmd = QString("SELECT DESKTOP_FILE_PATH,LOCAL_NAME,ICON FROM appInfo WHERE LOCAL_NAME OR NAME_EN OR NAME_ZH OR PINYIN_NAME OR FIRST_LETTER_OF_PINYIN LIKE '%%0%' ORDER BY FAVORITES DESC")
                    .arg(keyWord);
        }

        if (sql.exec(cmd)) {
            while (sql.next()) {
                installAppInfoRes << sql.value(0).toString() << sql.value(1).toString() << sql.value(2).toString();
            }
        } else {
            qWarning() << QString("cmd %0 failed!").arg(cmd) << m_database->lastError();
            res = false;
        }
        if (!m_database->commit()) {
            qWarning() << "Failed to commit !" << cmd;
            m_database->rollback();
            res = false;
        }
    } else {
        qWarning() << "Failed to start transaction mode!!!";
        res = false;
    }
    return res;
}

bool AppInfoTablePrivate::searchInstallApp(QStringList &keyWord, QStringList &installAppInfoRes)
{
    bool res(true);
    if (m_database->transaction() or keyWord.size() != 0) {
        QSqlQuery sql(*m_database);
        QString cmd;
        if (keyWord.at(0).size() < 2) {
            cmd = QString("SELECT DESKTOP_FILE_PATH,LOCAL_NAME,ICON,NAME_EN,NAME_ZH,FIRST_LETTER_OF_PINYIN FROM appInfo"
                          " WHERE LOCAL_NAME LIKE '%%0%' OR NAME_EN LIKE '%%0%' OR NAME_ZH LIKE '%%0%' OR FIRST_LETTER_OF_PINYIN LIKE '%%0%'")
                    .arg(keyWord.at(0));
        } else {
            cmd = QString("SELECT DESKTOP_FILE_PATH,LOCAL_NAME,ICON,NAME_EN,NAME_ZH,FIRST_LETTER_OF_PINYIN FROM appInfo"
                          " WHERE LOCAL_NAME LIKE '%%0%' OR NAME_EN LIKE '%%0%' OR NAME_ZH LIKE '%%0%' OR PINYIN_NAME LIKE '%%0%' OR FIRST_LETTER_OF_PINYIN LIKE '%%0%'")
                    .arg(keyWord.at(0));
        }
        for (int i = 0; ++i<keyWord.size();) {
            if (keyWord.at(i).size() < 2) {
                cmd += QString(" AND (LOCAL_NAME LIKE '%%1%' OR NAME_EN LIKE '%%1%' OR NAME_ZH LIKE '%%1%' OR FIRST_LETTER_OF_PINYIN LIKE '%%1%')")
                        .arg(keyWord.at(i));
            } else {
                cmd += QString(" AND (LOCAL_NAME LIKE '%%1%' OR NAME_EN LIKE '%%1%' OR NAME_ZH LIKE '%%1%' OR PINYIN_NAME LIKE '%%1%' OR FIRST_LETTER_OF_PINYIN LIKE '%%1%')")
                        .arg(keyWord.at(i));
            }
        }
        cmd += QString(" ORDER BY LENGTH(LOCAL_NAME)");
        if (sql.exec(cmd)) {
            while (sql.next()) {
                installAppInfoRes << sql.value(0).toString() << sql.value(1).toString() << sql.value(2).toString();
            }
        } else {
            qWarning() << QString("cmd %0 failed!").arg(cmd) << m_database->lastError();
            res = false;
        }
        if (!m_database->commit()) {
            qWarning() << "Failed to commit !" << cmd;
            m_database->rollback();
            res = false;
        }
    } else {
        qWarning() << "Failed to start transaction mode!!! keyword size:" << keyWord.size();
        res = false;
    }
    return res;
}

bool AppInfoTablePrivate::uninstallApp(QString &desktopfp)
{
    bool res(false);

    bool isOsReleaseUbuntu(false);
    QFile file("/etc/os-release");
    if (file.open(QFile::ReadOnly)) {
        QByteArray line = file.readLine();
        file.close();

        if (QString(line).contains("Ubuntu")) { //目前已无效
            isOsReleaseUbuntu = true;
        }
    }
    QString cmd;
    QProcess process;
    if (!isOsReleaseUbuntu) {
        cmd = QString("kylin-uninstaller %1")
                      .arg(desktopfp.toLocal8Bit().data());
        res = QProcess::startDetached(cmd);
        qDebug() << "kylin-uninstaller uninstall：" << cmd << res;
    } else {
        cmd = QString("dpkg -S " + desktopfp);
        process.start("sh", QStringList() << "-c" << cmd);
        process.waitForFinished();
        QString output = process.readAllStandardOutput().trimmed();
        QString packageName = output.split(":").at(0);
        cmd = QString("kylin-installer -remove %0")
              .arg(packageName.toLocal8Bit().data());
        res = QProcess::startDetached(cmd);
        qDebug() << "dpkg -S uninstall：" << cmd << res;
    }

    return res;
}

QString AppInfoTablePrivate::lastError() const
{
    return m_database->lastError().text();
}

AppInfoTablePrivate::~AppInfoTablePrivate()
{
    this->closeDataBase();
}

bool AppInfoTablePrivate::initDateBaseConnection()
{
    m_database->setDatabaseName(APP_DATABASE_PATH + APP_DATABASE_NAME);
    if(!m_database->open()) {
        qWarning() << m_database->lastError();
        return false;
//        QApplication::quit();
    }
    return true;
}

bool AppInfoTablePrivate::openDataBase()
{
    *m_database = QSqlDatabase::addDatabase("QSQLITE", m_ConnectionName);
    m_database->setDatabaseName(APP_DATABASE_PATH + APP_DATABASE_NAME);

    if(!m_database->open()) {
        return false;
    }
    return true;
}

void AppInfoTablePrivate::closeDataBase()
{
    m_database->close();
    delete m_database;
    QSqlDatabase::removeDatabase(m_ConnectionName);
}

AppInfoTable::AppInfoTable(QObject *parent) : QObject(parent), d(new AppInfoTablePrivate(this))
{
}

bool AppInfoTable::setAppFavoritesState(QString &desktopfp, size_t num)
{
    return d->setAppFavoritesState(desktopfp, num);
}

bool AppInfoTable::setAppTopState(QString &desktopfp, size_t num)
{
    return d->setAppTopState(desktopfp, num);
}

bool AppInfoTable::setAppLaunchTimes(QString &desktopfp, size_t num)
{
    return d->setAppLaunchTimes(desktopfp, num);
}

bool AppInfoTable::updateAppLaunchTimes(QString &desktopfp)
{
    return d->updateAppLaunchTimes(desktopfp);
}

bool AppInfoTable::setAppLockState(QString &desktopfp, size_t num)
{
    return d->setAppLockState(desktopfp, num);
}

bool AppInfoTable::getAllAppDesktopList(QStringList &list)
{
    return d->getAllAppDesktopList(list);
}

bool AppInfoTable::getFavoritesAppList(QStringList &list)
{
    return d->getFavoritesAppList(list);
}

bool AppInfoTable::getTopAppList(QStringList &list)
{
    return d->getTopAppList(list);
}

bool AppInfoTable::getLaunchTimesAppList(QStringList &list)
{
    return d->getLaunchTimesAppList(list);
}

bool AppInfoTable::getAppCategory(QString &desktopfp, QString &category)
{
    return d->getAppCategory(desktopfp, category);
}

bool AppInfoTable::getAppInfoResults(QVector<AppInfoTable::AppInfoResult> &appInfoResults)
{
    return d->getAppInfoResults(appInfoResults);
}

bool AppInfoTable::getAppLockState(QString &desktopfp, size_t &num)
{
    return d->getAppLockState(desktopfp, num);
}

bool AppInfoTable::getAppTopState(QString &desktopfp, size_t &num)
{
    return d->getAppTopState(desktopfp, num);
}

bool AppInfoTable::getAppLaunchedState(QString &desktopfp, size_t &num)
{
    return d->getAppLaunchedState(desktopfp, num);
}

bool AppInfoTable::getAppFavoriteState(QString &desktopfp, size_t &num)
{
    return d->getAppFavoriteState(desktopfp, num);
}

bool AppInfoTable::addAppShortcut2Desktop(QString &desktopfp)
{
    return d->addAppShortcut2Desktop(desktopfp);
}

bool AppInfoTable::addAppShortcut2Panel(QString &desktopfp)
{
    return d->addAppShortcut2Panel(desktopfp);
}

bool AppInfoTable::searchInstallApp(QString &keyWord, QStringList &installAppInfoRes)
{
    return d->searchInstallApp(keyWord, installAppInfoRes);
}

bool AppInfoTable::searchInstallApp(QStringList &keyWord, QStringList &installAppInfoRes)
{
    return d->searchInstallApp(keyWord, installAppInfoRes);
}

bool AppInfoTable::uninstallApp(QString &desktopfp)
{
    return d->uninstallApp(desktopfp);
}

QString AppInfoTable::lastError() const
{
    return d->lastError();
}
