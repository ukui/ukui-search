#include <qt5xdg/XdgDesktopFile>
#include <QMutexLocker>
#include <QCryptographicHash>
#include <QFile>
#include "app-db-manager.h"
#include "../libsearch/file-utils.h"
using namespace UkuiSearch;
#define GENERAL_APP_DESKTOP_PATH "/usr/share/applications/"
#define ANDROID_APP_DESKTOP_PATH QDir::homePath() + "/.local/share/applications/"
#define SNAPD_APP_DESKTOP_PATH "/var/lib/snapd/desktop/applications/"

static AppDBManager *global_instance = AppDBManager::getInstance();
QMutex AppDBManager::s_installAppMapMutex;
AppDBManager *AppDBManager::getInstance()
{
    if (!global_instance) {
        global_instance = new AppDBManager();
    }
    return global_instance;
}
AppDBManager::AppDBManager(QObject *parent) : QObject(parent), m_database(new QSqlDatabase)
{
    openDataBase();
    m_watchAppDir = new QFileSystemWatcher(this);
    m_watchAppDir->addPath(GENERAL_APP_DESKTOP_PATH);
    QDir androidPath(ANDROID_APP_DESKTOP_PATH);
    if(!androidPath.exists()) {
        androidPath.mkpath(ANDROID_APP_DESKTOP_PATH);
    }
    m_watchAppDir->addPath(ANDROID_APP_DESKTOP_PATH);

    QDir snapdPath(SNAPD_APP_DESKTOP_PATH);
    if(!snapdPath.exists()) {
        snapdPath.mkpath(SNAPD_APP_DESKTOP_PATH);
    }
    m_watchAppDir->addPath(SNAPD_APP_DESKTOP_PATH);

    initDateBaseConnection();

    connect(m_watchAppDir, &QFileSystemWatcher::directoryChanged, this, [ = ](const QString & path) {
        qDebug() << "m_watchAppDir directoryChanged:" << path;
        if (m_database->transaction()) {
            this->updateAppInfoDB();
            if (!m_database->commit()) {
                qWarning() << "Failed to commit !";
                m_database->rollback();
            }
        } else {
            qWarning() << "Failed to start transaction mode!!!";
        }
    });
}

AppDBManager::~AppDBManager()
{
    if(m_watchAppDir) {
        delete m_watchAppDir;
    }
    m_watchAppDir = NULL;
    closeDataBase();
}

void AppDBManager::buildAppInfoDB()
{
    QSqlQuery sql(*m_database);
    QString cmd = QString("CREATE TABLE IF NOT EXISTS appInfo(%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19,%20)")
//                         .arg("ID INT")//自增id
                         .arg("DESKTOP_FILE_PATH TEXT")//desktop文件路径
                         .arg("MODIFYED_TIME TEXT")//YYYYMMDDHHmmSS 修改日期
                         .arg("INSERT_TIME TEXT")//YYYYMMDDHHmmSS 插入日期
                         .arg("LOCAL_NAME TEXT")//本地名称，跟随系统语言
                         .arg("NAME_EN TEXT")//应用英文名称
                         .arg("NAME_ZH TEXT")//应用中文名称
                         .arg("PINYIN_NAME TEXT")//中文拼音
                         .arg("FIRST_LETTER_OF_PINYIN TEXT")//中文拼音首字母
                         .arg("ICON TEXT")//图标名称（或路径）
                         .arg("TYPE TEXT")//应用类型
                         .arg("CATEGORY TEXT")//应用分类
                         .arg("EXEC TEXT")//应用命令
                         .arg("COMMENT TEXT")//应用注释
                         .arg("MD5 TEXT")//desktop文件内容md5值
                         .arg("LAUNCH_TIMES INT")//应用打开次数, 等比例缩减
                         .arg("FAVORITES INT")//收藏顺序0:为收藏，>0的数字表示收藏顺序
                         .arg("LAUNCHED INT")//应用安装后是否打开过0:未打开过;1:打开过
                         .arg("TOP INT")//置顶顺序 0:未置顶；>0的数字表示置顶顺序
                         .arg("LOCK INT")//应用是否锁定（管控），0未锁定，1锁定
                         .arg("PRIMARY KEY (DESKTOP_FILE_PATH)");

    if (!sql.exec(cmd)) {
        qWarning() << m_database->lastError() << cmd;
        return;
    }
}

void AppDBManager::updateAppInfoDB()
{
    QMutexLocker locker(&s_installAppMapMutex);
    m_installAppMap.clear();
    this->getAllDesktopFilePath(GENERAL_APP_DESKTOP_PATH);
    this->getAllDesktopFilePath(ANDROID_APP_DESKTOP_PATH);
    this->getAllDesktopFilePath(SNAPD_APP_DESKTOP_PATH);
    QStringList filePathList;
    this->getFilePathList(filePathList);
    QSqlQuery sql(*m_database);
    QString cmd;
    if (!sql.exec("SELECT COUNT(*) FROM appInfo")) {
        this->buildAppInfoDB();
        for (auto &filePath : filePathList) {
            this->addAppDesktopFile2DB(filePath);
        }
    } else {
        cmd = QString("SELECT COUNT(*) FROM appInfo");
        if (sql.exec(cmd)) {
            if (sql.next()) {
                if (sql.value(0).toInt() > filePathList.size()) {
                    int size = sql.value(0).toInt();
                    cmd = QString("SELECT DESKTOP_FILE_PATH FROM appInfo");
                    if (!sql.exec(cmd)) {
                        qWarning() << m_database->lastError() << cmd;
                        return;
                    }
                    QString path;
                    for (int i = 0; i<size; ++i) {
                        if (!sql.next()) {
                            qWarning() << m_database->lastError() << cmd;
                            return;
                        }
                        path = sql.value(0).toString();
                        if (!filePathList.contains(path)) {
                            this->deleteAppDesktopFile2DB(path);
                            break;
                        }
                    }
                }
                for (QString &filePath:filePathList) {
                    cmd = QString("SELECT COUNT(*) FROM appInfo WHERE DESKTOP_FILE_PATH = '%0'").arg(filePath);
                    if (sql.exec(cmd)) {
                        if (sql.next()) {
                            if (sql.value(0).toInt() == 0) {
                                this->addAppDesktopFile2DB(filePath);
                            } else {
                                cmd = QString("SELECT MD5 FROM appInfo WHERE DESKTOP_FILE_PATH = '%0'").arg(filePath);
                                if (!sql.exec(cmd)) {
                                    qWarning() << m_database->lastError() << cmd;
                                    return;
                                }
                                if (!sql.next()) {
                                    qWarning() << m_database->lastError() << cmd;
                                    return;
                                }
                                if (sql.value(0).toString() != getAppDesktopMd5(filePath)) {
                                    this->updateAppDesktopFile2DB(filePath);
                                }
                            }
                        } else {
                            qWarning() << m_database->lastError() << cmd;
                            return;
                        }
                    } else {
                        qWarning() << m_database->lastError() << cmd;
                        return;
                    }
                }
            } else {
                qWarning() << m_database->lastError() << cmd;
                return;
            }
        } else {
            qWarning() << m_database->lastError() << cmd;
            return;
        }
    }
}

void AppDBManager::getFilePathList(QStringList &pathList)
{
    for (auto i=m_installAppMap.begin(); i!=m_installAppMap.end(); ++i) {
        pathList.append(i.value().at(0));
    }
}

/**
 * @brief AppMatch::getAllDesktopFilePath 遍历所有desktop文件
 * @param path 存放desktop文件夹
 */
void AppDBManager::getAllDesktopFilePath(QString path) {

    QDir dir(path);
    if(!dir.exists()) {
        return;
    }
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::DirsFirst);
    QFileInfoList list = dir.entryInfoList();
    list.removeAll(QFileInfo("/usr/share/applications/screensavers"));
    if(list.size() < 1) {
        return;
    }
    XdgDesktopFile desktopfile;
    int i = 0;
    while(i < list.size()) {
        QFileInfo fileInfo = list.at(i);
        //如果是文件夹，递归
        bool isDir = fileInfo.isDir();
        if(isDir) {
            getAllDesktopFilePath(fileInfo.filePath());
            qDebug() << fileInfo.filePath();
            ++i;
        } else {
            QString filePathStr = fileInfo.filePath();
            if(m_excludedDesktopfiles.contains(filePathStr)) {
                ++i;
                continue;
            }

            //过滤后缀不是.desktop的文件
            if(!filePathStr.endsWith(".desktop")) {
                ++i;
                continue;
            }

            desktopfile.load(filePathStr);
            if(desktopfile.value("NoDisplay").toString().contains("true") || desktopfile.value("NotShowIn").toString().contains("UKUI")) {
                ++i;
                continue;
            }

            QString name = desktopfile.localizedValue("Name").toString();
            if(name.isEmpty()) {
                ++i;
                qDebug() << filePathStr << "name!!";
                continue;
            }

            QString icon = desktopfile.iconName();
            NameString appname;
            QStringList appInfolist;

            appname.app_name = name;
            appInfolist << filePathStr << icon;
            appInfolist.append(desktopfile.value("Name").toString());
            appInfolist.append(desktopfile.value("Name[zh_CN]").toString());
            m_installAppMap.insert(appname, appInfolist);
            ++i;
        }
    }
}

void AppDBManager::initDateBaseConnection()
{
    if (m_database->transaction()) {
        this->updateAppInfoDB();
        if (!m_database->commit()) {
            qWarning() << "Failed to commit !";
            m_database->rollback();
        }
    } else {
        qWarning() << "Failed to start transaction mode!!!";
    }
}

void AppDBManager::openDataBase()
{
    QDir dir;
    if (!dir.exists(APP_DATABASE_PATH)) {
        dir.mkpath(APP_DATABASE_PATH);
    }
    if (QSqlDatabase::contains(CONNECTION_NAME)) {
        *m_database = QSqlDatabase::database(CONNECTION_NAME);
    } else {
        *m_database = QSqlDatabase::addDatabase("QSQLITE", CONNECTION_NAME);
        m_database->setDatabaseName(APP_DATABASE_PATH + APP_DATABASE_NAME);
    }
    if(!m_database->open()) {
        qWarning() << m_database->lastError();
        QApplication::quit();
    }
}

void AppDBManager::closeDataBase()
{
    m_database->close();
    delete m_database;
    QSqlDatabase::removeDatabase(CONNECTION_NAME);
}

QString AppDBManager::getAppDesktopMd5(QString &desktopfd)
{
    QString res;
    QFile file(desktopfd);
    file.open(QIODevice::ReadOnly);
    res = QString::fromStdString(QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5).toHex().toStdString());
    file.close();
    return res;
}

void AppDBManager::getInstallAppMap(QMap<QString, QStringList> &installAppMap)
{
    QMutexLocker locker(&s_installAppMapMutex);
    for (auto i=m_installAppMap.begin(); i!=m_installAppMap.end(); ++i) {
        installAppMap[i.key().app_name] = i.value();
    }
    installAppMap.detach();
}

bool AppDBManager::addAppDesktopFile2DB(QString &desktopfd)
{
    bool res(true);
    QSqlQuery sql(*m_database);
    XdgDesktopFile desktopfile;
    desktopfile.load(desktopfd);
    QString hanzi, pinyin, firstLetterOfPinyin;
    bool isHanzi = true;
    if (desktopfile.contains("Name[zh_CN]")) {
        hanzi = desktopfile.value("Name[zh_CN]").toString();
    } else {
        hanzi = desktopfile.value("Name").toString();
        if (!hanzi.contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
            isHanzi = false;
        }
    }
    if (isHanzi) {
        QStringList pinyinList = FileUtils::findMultiToneWords(hanzi);
        for (int i = 0; i<pinyinList.size(); ++i) {
            if (i%2) {
                firstLetterOfPinyin += pinyinList.at(i);
            } else {
                pinyin += pinyinList.at(i);
            }
        }
    }

    QString cmd = QString("INSERT INTO appInfo "
                          "(DESKTOP_FILE_PATH,MODIFYED_TIME,INSERT_TIME,LOCAL_NAME,NAME_EN,NAME_ZH,PINYIN_NAME,FIRST_LETTER_OF_PINYIN,ICON,TYPE,CATEGORY,EXEC,COMMENT,MD5,LAUNCH_TIMES,FAVORITES,LAUNCHED,TOP,LOCK) "
                          "VALUES('%1','%2','%3','%4','%5','%6','%7','%8','%9','%10','%11','%12','%13','%14',%15,%16,%17,%18,%19)")
            .arg(desktopfd)
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
            .arg(desktopfile.localizedValue("Name").toString())
            .arg(desktopfile.value("Name").toString())
            .arg(hanzi)
            .arg(pinyin)
            .arg(firstLetterOfPinyin)
            .arg(desktopfile.value("Icon").toString())
            .arg(desktopfile.value("Type").toString())
            .arg(desktopfile.value("Categories").toString())
            .arg(desktopfile.value("Exec").toString())
            .arg(desktopfile.value("Comment").toString())
            .arg(getAppDesktopMd5(desktopfd))
            .arg(0)
            .arg(0)
            .arg(0)
            .arg(0)
            .arg(0);
    if (!sql.exec(cmd)) {
        qWarning() << m_database->lastError() << cmd;
        res = false;
    }
    if (res) {
        qDebug() << "app database add " << desktopfd << "success!";
    } else {
        qDebug() << "app database add " << desktopfd << "failed!";
    }
    return res;
}

bool AppDBManager::deleteAppDesktopFile2DB(QString &desktopfd)
{
    bool res(true);
    QSqlQuery sql(*m_database);
    QString cmd = QString("DELETE FROM appInfo WHERE DESKTOP_FILE_PATH = '%0'").arg(desktopfd);
    if (!sql.exec(cmd)) {
        qWarning() << m_database->lastError() << cmd;
        res = false;
    }
    if (res) {
        qDebug() << "app database delete " << desktopfd << "success!";
    } else {
        qDebug() << "app database delete " << desktopfd << "failed!";
    }
    return res;
}

bool AppDBManager::updateAppDesktopFile2DB(QString &desktopfd)
{
    bool res(true);
    XdgDesktopFile desktopfile;
    desktopfile.load(desktopfd);
    if(desktopfile.value("NoDisplay").toString().contains("true") || desktopfile.value("NotShowIn").toString().contains("UKUI")) {
        qDebug() << "app" << desktopfd << "is changed, NoDisplay or NotShowIn is working!";
        return this->deleteAppDesktopFile2DB(desktopfd);
    }
    QString hanzi, pinyin, firstLetterOfPinyin;
    bool isHanzi = true;
    if (desktopfile.contains("Name[zh_CN]")) {
        hanzi = desktopfile.value("Name[zh_CN]").toString();
    } else {
        hanzi = desktopfile.value("Name").toString();
        if (!hanzi.contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
            isHanzi = false;
        }
    }
    if (isHanzi) {
        QStringList pinyinList = FileUtils::findMultiToneWords(hanzi);
        for (int i = 0; i<pinyinList.size(); ++i) {
            if (i%2) {
                firstLetterOfPinyin += pinyinList.at(i);
            } else {
                pinyin += pinyinList.at(i);
            }
        }
    }
    QSqlQuery sql(*m_database);
    QString cmd = QString("UPDATE appInfo SET "
                          "MODIFYED_TIME='%0',"
                          "LOCAL_NAME='%1',"
                          "NAME_EN='%2',"
                          "NAME_ZH='%3'"
                          ",PINYIN_NAME='%4',"
                          "FIRST_LETTER_OF_PINYIN='%5',"
                          "ICON='%6',"
                          "TYPE='%7',"
                          "CATEGORY='%8',"
                          "EXEC='%9',"
                          "COMMENT='%10',"
                          "MD5='%11' "
                          "WHERE DESKTOP_FILE_PATH='%12'")
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
            .arg(desktopfile.localizedValue("Name").toString())
            .arg(desktopfile.value("Name").toString())
            .arg(hanzi)
            .arg(pinyin)
            .arg(firstLetterOfPinyin)
            .arg(desktopfile.value("Icon").toString())
            .arg(desktopfile.value("Type").toString())
            .arg(desktopfile.value("Categories").toString())
            .arg(desktopfile.value("Exec").toString())
            .arg(desktopfile.value("Comment").toString())
            .arg(getAppDesktopMd5(desktopfd))
            .arg(desktopfd);
    if (!sql.exec(cmd)) {
        qWarning() << m_database->lastError() << cmd;
        res = false;
    }
    if (res) {
        qDebug() << "app database update " << desktopfd << "success!";
    } else {
        qDebug() << "app database update " << desktopfd << "failed!";
    }
    return res;
}

bool AppDBManager::updateAppLaunchTimes(QString &desktopfp)
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


