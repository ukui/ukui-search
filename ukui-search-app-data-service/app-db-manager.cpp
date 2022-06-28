#include "app-db-manager.h"
#include "file-utils.h"
#include "convert-winid-to-desktop.h"

#include <qt5xdg/XdgDesktopFile>
#include <QMutexLocker>
#include <QCryptographicHash>
#include <QFile>

#define GENERAL_APP_DESKTOP_PATH "/usr/share/applications/"
#define ANDROID_APP_DESKTOP_PATH QDir::homePath() + "/.local/share/applications/"
#define SNAPD_APP_DESKTOP_PATH "/var/lib/snapd/desktop/applications/"

#define LAST_LOCALE_NAME QDir::homePath() + "/.config/org.ukui/ukui-search/appdata/last-locale-name.conf"
#define LOCALE_NAME_VALUE "CurrentLocaleName"

using namespace UkuiSearch;

static AppDBManager *global_instance;
QMutex AppDBManager::s_installAppMapMutex;

AppDBManager *AppDBManager::getInstance()
{
    if (!global_instance) {
        global_instance = new AppDBManager();
    }
    return global_instance;
}

AppDBManager::AppDBManager(QObject *parent) : QObject(parent), m_database(QSqlDatabase())
{
    //链接数据库
    if (openDataBase()) {
        //监听系统语言变化
        m_qSettings = new QSettings(LAST_LOCALE_NAME, QSettings::IniFormat);
        m_localeChanged = false;
        m_qSettings->beginGroup(LOCALE_NAME_VALUE);
        QString lastLocale = m_qSettings->value(LOCALE_NAME_VALUE).toString();
        if (QLocale::system().name().compare(lastLocale)) {
            qDebug() << "I'm going to update the locale name in conf file.";
            if (!lastLocale.isEmpty()) {
                m_localeChanged = true;
            }
            m_qSettings->setValue(LOCALE_NAME_VALUE, QLocale::system().name());
        }
        m_qSettings->endGroup();

        //初始化数据库
        initDataBase();

        //初始化FileSystemWatcher
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

        //监听desktop文件所在目录,TODO:directoryChange会发多次信号，需要计时器阻塞
        connect(m_watchAppDir, &QFileSystemWatcher::directoryChanged, this, [ = ](const QString & path) {
            qDebug() << "m_watchAppDir directoryChanged:" << path;
            if (m_database.transaction()) {
//                this->updateAppInfoDB();
                this->updateAllData2DB();
                if (!m_database.commit()) {
                    qWarning() << "Failed to commit !";
                    m_database.rollback();
                } else {
                    Q_EMIT this->finishHandleAppDB();
                }
            } else {
                qWarning() << "Failed to start transaction mode!!!";
            }
        });

        //监控应用进程开启
        connect(KWindowSystem::self(), &KWindowSystem::windowAdded, [ = ](WId id) {
            QString desktopfp = ConvertWinidToDesktop::getConverter().tranIdToDesktop(id);
            if (!desktopfp.isEmpty()) {
                AppDBManager::getInstance()->updateAppLaunchTimes(desktopfp);
            }
        });
    } else {
        qDebug() << "App-db-manager does nothing.";
    }
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
    QSqlQuery sql(m_database);
    QString cmd = QString("CREATE TABLE IF NOT EXISTS appInfo(%1, %2, %3, %4, %5, %6, %7, %8,%9, %10, %11, %12, %13, %14, %15, %16, %17, %18, %19, %20, %21)")
//                         .arg("ID INT")//自增id
                         .arg("DESKTOP_FILE_PATH TEXT")//desktop文件路径
                         .arg("MODIFYED_TIME TEXT")//YYYYMMDDHHmmSS 修改日期
                         .arg("INSERT_TIME TEXT")//YYYYMMDDHHmmSS 插入日期
                         .arg("LOCAL_NAME TEXT")//本地名称，跟随系统语言
                         .arg("NAME_EN TEXT")//应用英文名称
                         .arg("NAME_ZH TEXT")//应用中文名称
                         .arg("PINYIN_NAME TEXT")//中文拼音
                         .arg("FIRST_LETTER_OF_PINYIN TEXT")//中文拼音首字母
                         .arg("FIRST_LETTER_ALL")//拼音和英文全拼
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
        qWarning() << m_database.lastError() << cmd;
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
    QSqlQuery sql(m_database);
    QString cmd = "SELECT COUNT(*) FROM appInfo";
    if (!sql.exec(cmd)) {
        this->buildAppInfoDB();
        for (auto &filePath : filePathList) {
            this->addAppDesktopFile2DB(filePath);
        }
    } else {
        if (sql.exec(cmd)) {
            if (sql.next()) {
                //删除多余项
                if (sql.value(0).toInt() > filePathList.size()) {
                    int size = sql.value(0).toInt();
                    cmd = QString("SELECT DESKTOP_FILE_PATH FROM appInfo");
                    if (!sql.exec(cmd)) {
                        qWarning() << m_database.lastError() << cmd;
                        return;
                    }
                    QString path;
                    for (int i = 0; i<size; ++i) {
                        if (!sql.next()) {
                            qWarning() << m_database.lastError() << cmd;
                            return;
                        }
                        path = sql.value(0).toString();
                        if (!filePathList.contains(path)) {
                            this->deleteAppDesktopFile2DB(path);
                            break;
                        }
                    }
                }
                //添加新增项，根据md5判断desktop文件是否改变以更新对应项
                for (QString &filePath : filePathList) {
                    cmd = QString("SELECT COUNT(*) FROM appInfo WHERE DESKTOP_FILE_PATH = '%0'").arg(filePath);
                    if (sql.exec(cmd)) {
                        if (sql.next()) {
                            if (sql.value(0).toInt() == 0) {
                                this->addAppDesktopFile2DB(filePath);
                            } else {
                                cmd = QString("SELECT MD5 FROM appInfo WHERE DESKTOP_FILE_PATH = '%0'").arg(filePath);
                                if (!sql.exec(cmd)) {
                                    qWarning() << m_database.lastError() << cmd;
                                    return;
                                }
                                if (!sql.next()) {
                                    qWarning() << m_database.lastError() << cmd;
                                    return;
                                }
                                if (sql.value(0).toString() != getAppDesktopMd5(filePath)) {
                                    this->updateAppDesktopFile2DB(filePath);
                                }
                            }
                        } else {
                            qWarning() << m_database.lastError() << cmd;
                            return;
                        }
                    } else {
                        qWarning() << m_database.lastError() << cmd;
                        return;
                    }
                }
            } else {
                qWarning() << m_database.lastError() << cmd;
                return;
            }
        } else {
            qWarning() << m_database.lastError() << cmd;
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

void AppDBManager::loadDesktopFilePaths(QString path, QFileInfoList &infolist)
{
    QDir dir(path);
    dir.setSorting(QDir::DirsFirst);
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    infolist.append(dir.entryInfoList());
}

void AppDBManager::updateAllData2DB()
{
    bool firstExec = false;
    QSqlQuery sql(m_database);
    QString cmd = "SELECT DESKTOP_FILE_PATH,MD5 FROM appInfo";
    QMap<QString, QString> dataMap;
    if (!sql.exec(cmd)) {
        qDebug() << "Fail to read database, because: " << m_database.lastError();
        this->buildAppInfoDB();
        firstExec = true;
    } else {
        sql.exec(cmd);
        while (sql.next()) {
            dataMap.insert(sql.value("DESKTOP_FILE_PATH").toString(), sql.value("MD5").toString());
        }
    }

    //遍历desktop文件
    QFileInfoList infos;
    this->loadDesktopFilePaths(GENERAL_APP_DESKTOP_PATH, infos);
    this->loadDesktopFilePaths(ANDROID_APP_DESKTOP_PATH, infos);
    this->loadDesktopFilePaths(SNAPD_APP_DESKTOP_PATH, infos);
    if(infos.size() < 1) {
        return;
    }
    XdgDesktopFile desktopfile;
    for (int i = 0; i < infos.length(); i++) {
        QFileInfo fileInfo = infos.at(i);
        QString path = fileInfo.filePath();
        //对目录递归
        if (fileInfo.isDir()) {
            loadDesktopFilePaths(path, infos);
            continue;
        }
        //排除非法路径（黑名单 + 非desktop文件）
        if (m_excludedDesktopfiles.contains(path) or !path.endsWith(".desktop")) {
            continue;
        }

        desktopfile.load(path);
        //排除NoDisplay和NotShowIn字段，排除loaclized名字为空
        if (desktopfile.value("NoDisplay").toString().contains("true") or
                desktopfile.value("NotShowIn").toString().contains("UKUI") or
                desktopfile.localizedValue("Name").toString().isEmpty()) {
            continue;
        }

        if (!firstExec) {
            //数据库有记录
            if (dataMap.contains(path)) {
                if (!QString::compare(dataMap.value(path), getAppDesktopMd5(path))) {
                    //判断系统语言是否改变
                    if (m_localeChanged) {
                        this->updateLocaleData2DB(path);
                    }
                    dataMap.remove(path);
                    continue;
                } else {
                    //数据库有记录但md5值改变则update
                    this->updateAppDesktopFile2DB(path);
                    dataMap.remove(path);
                    continue;
                }
            } else {
                //数据库中没有记录则insert
                this->addAppDesktopFile2DB(path);
                dataMap.remove(path);
                continue;
            }
        }
        //数据库为空则全部insert
        this->addAppDesktopFile2DB(path);
        dataMap.remove(path);
    }

    //遍历完成后重置标志位
    m_localeChanged = false;

    //数据库冗余项直接delete
    if (!dataMap.isEmpty()) {
        for (auto i = dataMap.constBegin(); i != dataMap.constEnd(); i++) {
            this->deleteAppDesktopFile2DB(i.key());
        }
    }
}

bool AppDBManager::updateLocaleData2DB(QString desktopPath)
{
    bool res(true);
    XdgDesktopFile desktopFile;
    desktopFile.load(desktopPath);
    QString localName = desktopFile.localizedValue("Name", "NULL").toString();
    QString firstLetter2All = localName;

    if (localName.contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
        firstLetter2All = FileUtils::findMultiToneWords(localName).at(0);
    }


    QSqlQuery sql(m_database);
    QString cmd = QString("UPDATE appInfo SET "
                          "LOCAL_NAME = '%0', FIRST_LETTER_ALL = '%1'"
                          "WHERE DESKTOP_FILE_PATH='%2'")
                          .arg(localName)
                          .arg(firstLetter2All)
                          .arg(desktopPath);

    if (!sql.exec(cmd)) {
        qWarning() << m_database.lastError() << cmd;
        res = false;
    }
    if (res) {
        AppInfoResult result;
        result.appLocalName = localName;
        result.firstLetter = firstLetter2All;
        Q_EMIT this->appDBItemUpdate(result);
        qDebug() << "Already to update the locale app-data of " << desktopPath;
    } else {
        qDebug() << "Fail to update the locale app-data of " << desktopPath;
    }
    return res;
}

void AppDBManager::initDataBase()
{
    if (m_database.transaction()) {
//        this->updateAppInfoDB();
        this->updateAllData2DB();
        if (!m_database.commit()) {
            qWarning() << "Failed to commit !";
            m_database.rollback();
        } else {
            Q_EMIT this->finishHandleAppDB();
        }
    } else {
        qWarning() << "Failed to start transaction mode!!!";
    }
}

bool AppDBManager::openDataBase()
{
    bool res(true);

    QDir dir;
    if (!dir.exists(APP_DATABASE_PATH)) {
        dir.mkpath(APP_DATABASE_PATH);
    }
    if (QSqlDatabase::contains(CONNECTION_NAME)) {
        m_database = QSqlDatabase::database(CONNECTION_NAME);
    } else {
        m_database = QSqlDatabase::addDatabase("QSQLITE", CONNECTION_NAME);
        m_database.setDatabaseName(APP_DATABASE_PATH + APP_DATABASE_NAME);
    }
    if(!m_database.open()) {
        qWarning() << "Fail to open AppDataBase, because" << m_database.lastError();
        res = false;
    }
    return res;
}

void AppDBManager::closeDataBase()
{
    m_database.close();
//    delete m_database;
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
    QSqlQuery sql(m_database);
    XdgDesktopFile desktopfile;
    desktopfile.load(desktopfd);
    QString hanzi, pinyin, firstLetterOfPinyin;
    QString localName = desktopfile.localizedValue("Name", "NULL").toString();
    QString firstLetter2All = localName;
    bool isHanzi = true;

    if (localName.contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
        firstLetter2All = FileUtils::findMultiToneWords(localName).at(0);
    }

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
            if (i % 2) {
                firstLetterOfPinyin += pinyinList.at(i);
            } else {
                pinyin += pinyinList.at(i);
            }
        }
    }

    QString cmd = QString("INSERT INTO appInfo "
                          "(DESKTOP_FILE_PATH, MODIFYED_TIME, INSERT_TIME, "
                          "LOCAL_NAME, NAME_EN, NAME_ZH, PINYIN_NAME, "
                          "FIRST_LETTER_OF_PINYIN, FIRST_LETTER_ALL, "
                          "ICON, TYPE, CATEGORY, EXEC, COMMENT, MD5, "
                          "LAUNCH_TIMES, FAVORITES, LAUNCHED, TOP, LOCK) "
                          "VALUES('%1','%2','%3','%4','%5','%6','%7','%8','%9','%10','%11','%12','%13','%14','%15',%16,%17,%18,%19,%20)")
            .arg(desktopfd)
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
            .arg(localName)
            .arg(desktopfile.value("Name").toString())
            .arg(hanzi)
            .arg(pinyin)
            .arg(firstLetterOfPinyin)
            .arg(firstLetter2All)
            .arg(desktopfile.value("Icon").toString())
            .arg(desktopfile.value("Type").toString())
            .arg(desktopfile.value("Categories").toString())
            .arg(desktopfile.value("Exec").toString())
            .arg(desktopfile.value("Comment").toString().replace("'", "''"))
            .arg(getAppDesktopMd5(desktopfd))
            .arg(0)
            .arg(0)
            .arg(0)
            .arg(0)
            .arg(0);
    if (!sql.exec(cmd)) {
        qWarning() << m_database.lastError() << cmd;
        res = false;
    }
    if (res) {
        AppInfoResult result;
        result.desktopPath = desktopfd;
        result.iconName = desktopfile.value("Icon").toString();
        result.appLocalName = localName;
        result.firstLetter = firstLetter2All;
        result.category = desktopfile.value("Categories").toString();
        Q_EMIT this->appDBItemAdd(result);
        qDebug() << "app database add " << desktopfd << "success!";
    } else {
        qDebug() << "app database add " << desktopfd << "failed!";
    }
    return res;
}

bool AppDBManager::deleteAppDesktopFile2DB(const QString &desktopfd)
{
    bool res(true);
    QSqlQuery sql(m_database);
    QString cmd = QString("DELETE FROM appInfo WHERE DESKTOP_FILE_PATH = '%0'").arg(desktopfd);
    if (!sql.exec(cmd)) {
        qWarning() << m_database.lastError() << cmd;
        res = false;
    }
    if (res) {
        Q_EMIT this->appDBItemDelete(desktopfd);
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
    QString localName = desktopfile.localizedValue("Name", "NULL").toString();
    QString firstLetter2All = localName;
    bool isHanzi = true;

    if (localName.contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
        firstLetter2All = FileUtils::findMultiToneWords(localName).at(0);
    }

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
        firstLetter2All = pinyinList.at(0);
        for (int i = 0; i<pinyinList.size(); ++i) {
            if (i % 2) {
                firstLetterOfPinyin += pinyinList.at(i);
            } else {
                pinyin += pinyinList.at(i);
            }
        }
    }

    QSqlQuery sql(m_database);
    QString cmd = QString("UPDATE appInfo SET "
                          "MODIFYED_TIME='%0',"
                          "LOCAL_NAME='%1',"
                          "NAME_EN='%2',"
                          "NAME_ZH='%3'"
                          ",PINYIN_NAME='%4',"
                          "FIRST_LETTER_OF_PINYIN='%5',"
                          "FIRST_LETTER_ALL='%6',"
                          "ICON='%7',"
                          "TYPE='%8',"
                          "CATEGORY='%9',"
                          "EXEC='%10',"
                          "COMMENT='%11',"
                          "MD5='%12' "
                          "WHERE DESKTOP_FILE_PATH='%13'")
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
            .arg(localName)
            .arg(desktopfile.value("Name").toString())
            .arg(hanzi)
            .arg(pinyin)
            .arg(firstLetterOfPinyin)
            .arg(firstLetter2All)
            .arg(desktopfile.value("Icon").toString())
            .arg(desktopfile.value("Type").toString())
            .arg(desktopfile.value("Categories").toString())
            .arg(desktopfile.value("Exec").toString())
            .arg(desktopfile.value("Comment").toString().replace("'", "''"))
            .arg(getAppDesktopMd5(desktopfd))
            .arg(desktopfd);
    if (!sql.exec(cmd)) {
        qWarning() << m_database.lastError() << cmd;
        res = false;
    }
    if (res) {
        AppInfoResult result;
        result.desktopPath = desktopfd;
        result.iconName = desktopfile.value("Icon").toString();
        result.appLocalName = localName;
        result.firstLetter = firstLetter2All;
        result.category = desktopfile.value("Categories").toString();
        Q_EMIT this->appDBItemUpdate(result);
        qDebug() << "app database update " << desktopfd << "success!";
    } else {
        qDebug() << "app database update " << desktopfd << "failed!";
    }
    return res;
}

bool AppDBManager::updateAppLaunchTimes(QString &desktopfp)
{
    bool res(true);
    if (m_database.transaction()) {
        QSqlQuery sql(m_database);
        QString cmd = QString("SELECT LAUNCH_TIMES FROM appInfo WHERE DESKTOP_FILE_PATH='%1'").arg(desktopfp);
        if (sql.exec(cmd)) {
            if (sql.next()) {
                int launchTimes = sql.value(0).toInt() + 1;
                cmd = QString("UPDATE appInfo SET MODIFYED_TIME='%0', LAUNCH_TIMES=%1, LAUNCHED=%2 WHERE DESKTOP_FILE_PATH='%3'")
                        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                        .arg(launchTimes)
                        .arg(1)
                        .arg(desktopfp);
                if (!sql.exec(cmd)) {
                    qWarning() << "Set app favorites state failed!" << m_database.lastError();
                    res = false;
                } else {
                    AppInfoResult result;
                    result.launchTimes = launchTimes;
                    Q_EMIT this->appDBItemUpdate(result);
                }
            } else {
                qWarning() << "Failed to exec next!" << cmd;
                res = false;
            }
        } else {
            qWarning() << "Failed to exec:" << cmd;
            res = false;
        }
        if (!m_database.commit()) {
            qWarning() << "Failed to commit !" << cmd;
            m_database.rollback();
            res = false;
        } else {
            Q_EMIT this->finishHandleAppDB();
        }
    } else {
        qWarning() << "Failed to start transaction mode!!!";
        res = false;
    }
    return res;
}


