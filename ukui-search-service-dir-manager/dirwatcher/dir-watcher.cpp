#include "dir-watcher.h"

#include <QDebug>
#include <QDir>
#include <QDBusConnection>
#include <QDBusArgument>
#include <QThread>
#include <fstab.h>
#include <QMutexLocker>

#define CURRENT_INDEXABLE_DIR_SETTINGS QDir::homePath() + "/.config/org.ukui/ukui-search/ukui-search-current-indexable-dir.conf"
#define INDEXABLE_DIR_VALUE "IndexableDir"

static std::once_flag flag;
static DirWatcher *global_intance = nullptr;
QMutex DirWatcher::s_mutex;

DirWatcher::DirWatcher(QObject *parent) : QObject(parent)
{
    m_qSettings = new QSettings(CURRENT_INDEXABLE_DIR_SETTINGS, QSettings::IniFormat);
    initData();
    initDiskWatcher();
    m_adaptor = new DirWatcherAdaptor(this);
}

DirWatcher::~DirWatcher()
{
    if (m_volumeMonitor) {
        g_signal_handler_disconnect(m_volumeMonitor, m_mountAddHandle);
        g_signal_handler_disconnect(m_volumeMonitor, m_mountRemoveHandle);
        m_volumeMonitor = nullptr;
    }
    if(m_qSettings){
        delete m_qSettings;
    }
}

DirWatcher *DirWatcher::getDirWatcher()
{
    std::call_once(flag, [ & ] {
        global_intance = new DirWatcher();
    });
    return global_intance;
}

QStringList DirWatcher::currentIndexableDir()
{
    QMutexLocker locker(&s_mutex);
    m_qSettings->beginGroup(INDEXABLE_DIR_VALUE);
    m_indexableDirList = m_qSettings->value(INDEXABLE_DIR_VALUE).toStringList();
    m_qSettings->endGroup();
    QStringList indexableDirs = m_indexableDirList;
    return indexableDirs;
}

QStringList DirWatcher::currentBlackListOfIndex()
{
    QMutexLocker locker(&s_mutex);
    QStringList blackListOfIndex = m_blackListOfIndex;
    return blackListOfIndex;
}

void DirWatcher::handleIndexItemAppend(const QString &path, QStringList &blackList)
{
    if (m_indexableDirList.contains(path)) {
        qDebug() << QString("index path %1 is already added.").arg(path);
        return;
    }
    m_indexableDirList << path;
    m_qSettings->beginGroup(INDEXABLE_DIR_VALUE);
    m_qSettings->setValue(INDEXABLE_DIR_VALUE, m_indexableDirList);
    m_qSettings->endGroup();
    Q_EMIT this->appendIndexItem(path, blackList);
    qDebug() << "index path:" << path << "blacklist:" << blackList;
}

void DirWatcher::handleIndexItemRemove(const QString &path)
{
    m_indexableDirList.removeAll(path);
    m_qSettings->beginGroup(INDEXABLE_DIR_VALUE);
    m_qSettings->setValue(INDEXABLE_DIR_VALUE, m_indexableDirList);
    m_qSettings->endGroup();
}

/**
 * @brief DirWatcher::blackListOfDir 根据传入目录返回当前目录下的所有不可搜索目录，没有则返回空列表
 * @param dirPath 要搜索的目录
 */
QStringList DirWatcher::blackListOfDir(const QString &dirPath)
{
    //new TODO: Optimize the search algorithm.
    //There is no processing for the subvolumes.May be a bug.
    QStringList blackListOfDir;
    QMutexLocker locker(&s_mutex);
    for (auto t = m_repeatedlyMountedDeviceInfo.constBegin(); t != m_repeatedlyMountedDeviceInfo.constEnd(); t++) {
        QString topRepeatedMountPoint;
        for (QString mountPoint: t.value()) {
            if (mountPoint.startsWith(dirPath)) {
                if (topRepeatedMountPoint.isEmpty()) {
                    topRepeatedMountPoint = mountPoint;
                    continue;
                } else if (topRepeatedMountPoint.startsWith(mountPoint)) {
                    blackListOfDir.append(topRepeatedMountPoint);
                    topRepeatedMountPoint = mountPoint;
                } else {
                    blackListOfDir.append(mountPoint);
                }
            }
        }
    }
    for (auto i = m_infoOfSubvolume.constBegin(); i != m_infoOfSubvolume.constEnd(); i++) {
        QString mountPoint = i.value();
        QString spec = i.key();
        //排除搜索列表指定多个目录时子卷会重复包含的情况，比如同时指定/home和/data/home
        QString tmp = dirPath;
        if (dirPath.startsWith(mountPoint)) {
            blackListOfDir.append(tmp.replace(0, mountPoint.length(), spec));
        }
        if (dirPath.startsWith(spec)) {
            blackListOfDir.append(tmp.replace(0, spec.length(), mountPoint));
        }
    }
    return blackListOfDir;
}

void DirWatcher::appendBlackListItemOfIndex(const QString &path)
{
    QMutexLocker locker(&s_mutex);
    m_blackListOfIndex.append(path);
    m_blackListOfIndex = m_blackListOfIndex.toSet().toList();
}

void DirWatcher::appendBlackListItemOfIndex(const QStringList &pathList)
{
    QMutexLocker locker(&s_mutex);
    m_blackListOfIndex.append(pathList);
    m_blackListOfIndex = m_blackListOfIndex.toSet().toList();
}

void DirWatcher::removeBlackListItemOfIndex(const QString &path)
{
    QMutexLocker locker(&s_mutex);
    m_blackListOfIndex.removeAll(path);
}

void DirWatcher::removeBlackListItemOfIndex(const QStringList &pathList)
{
    QMutexLocker locker(&s_mutex);
    for (QString path: pathList) {
        m_blackListOfIndex.removeAll(path);
    }
}

void DirWatcher::initDbusService()
{
    QDBusConnection sessionBus = QDBusConnection::sessionBus();
    if (!sessionBus.registerObject("/org/ukui/search/fileindex", this, QDBusConnection::ExportAdaptors)){
        qWarning() << "ukui-search-fileindex dbus register object failed reason:" << sessionBus.lastError();
    }
}

QStringList DirWatcher::currentSearchableDir()
{
    QMutexLocker locker(&s_mutex);
    return m_searchableDirList;
}

QStringList DirWatcher::searchableDirForSearchApplication()
{
    QMutexLocker locker(&s_mutex);
    return m_searchableListForApplication;
}

void DirWatcher::mountAddCallback(GVolumeMonitor *monitor, GMount *gmount, DirWatcher *pThis)
{
    qDebug() << "Mount Added";
    pThis->handleDisk();
}

void DirWatcher::mountRemoveCallback(GVolumeMonitor *monitor, GMount *gmount, DirWatcher *pThis)
{
    qDebug() << "Mount Removed";
    QMutexLocker locker(&s_mutex);
    //处理u盘设备
    if (pThis->m_removedUDiskDevice != NULL) {
        pThis->m_currentUDiskDeviceInfo.remove(pThis->m_removedUDiskDevice);
        qDebug() << "m_currentMountedDeviceInfo:" << pThis->m_currentMountedDeviceInfo;
        qDebug() << "m_repeatedlyMountedDeviceInfo:" << pThis->m_repeatedlyMountedDeviceInfo;
        qDebug() << "m_currentUDiskDeviceInfo:" << pThis->m_currentUDiskDeviceInfo;
        qDebug() << "m_blackListOfIndex:" << pThis->m_blackListOfIndex;
        pThis->m_removedUDiskDevice = "";
        return;
    }

    GMount* mount = (GMount*)g_object_ref(gmount);
    GFile* rootFile;
    rootFile = g_mount_get_root(mount);
    if (!rootFile) {
        return;
    }
    QString mountPoint = g_file_get_uri(rootFile);
    if (mountPoint.isEmpty()) {
        return;
    }
    //处理uri转码,处理子卷情况
    if (mountPoint.startsWith("file:///")) {
        QString removedMountPoint = g_filename_from_uri(mountPoint.toUtf8().constData(), nullptr, nullptr);
        pThis->m_blackListOfIndex.removeAll(removedMountPoint);
        for (auto t = pThis->m_infoOfSubvolume.constBegin(); t != pThis->m_infoOfSubvolume.constEnd(); t++) {
            if (removedMountPoint.startsWith(t.value())) {
                pThis->m_blackListOfIndex.removeAll(removedMountPoint.replace(t.value(), t.key()));
            }
            if (removedMountPoint.startsWith(t.key())) {
                pThis->m_blackListOfIndex.removeAll(removedMountPoint.replace(t.key(), t.value()));
            }
        }
        qDebug() << "m_currentMountedDeviceInfo:" << pThis->m_currentMountedDeviceInfo;
        qDebug() << "m_repeatedlyMountedDeviceInfo:" << pThis->m_repeatedlyMountedDeviceInfo;
        qDebug() << "m_currentUDiskDeviceInfo:" << pThis->m_currentUDiskDeviceInfo;
        qDebug() << "m_blackListOfIndex:" << pThis->m_blackListOfIndex;
    } else {
        qWarning() << "There's someting wrong with the MountPoint!";
    }
    g_object_unref(rootFile);
}

void DirWatcher::appendIndexableListItem(const QString &path)
{
    QFile file(path);
    if (!file.exists()) {
        qWarning() << QString("target path:%1 is not exists!").arg(path);
        return;
    }
    if (path == "/") {
        this->currentIndexableDir();
        this->handleIndexItemAppend(path, m_blackListOfIndex);
        return;
    }

    QStringList blackList = this->blackListOfDir(path);
    //处理要添加索引的路径与索引黑名单中路径为父子关系的情况
    for (const QString& blackListPath : m_blackListOfIndex) {
        if (path.startsWith(blackListPath + "/")) {
            return;
        }
        if (blackListPath.startsWith(path + "/")) {
            blackList.append(blackListPath);
        }
    }

    this->currentIndexableDir();
    qDebug() << "current indexable dirs:" << m_indexableDirList;
    //处理要添加索引的路径与已索引路径为父子关系的情况
    for (int i = 0; i < m_indexableDirList.length(); i++) {
        const QString indexablePath = m_indexableDirList.at(i);
        if (path.startsWith(indexablePath + "/")) {
            qCritical() << QString("The parent of the path:%1 has been added.").arg(path);
            return;
        }
        if (blackList.contains(indexablePath)) {
            qCritical() << "current path is in repeat mounted devices and another path which is in the device that has the same dev num has been added to the indexable list.";
            return;
        }
        if (indexablePath.startsWith(path + "/")) {
            m_indexableDirList.removeAll(indexablePath);
            blackList.append(indexablePath);
        }
    }
    this->handleIndexItemAppend(path, blackList);
}

void DirWatcher::removeIndexableListItem(const QString &path)
{
    this->currentIndexableDir();
    this->handleIndexItemRemove(path);
    Q_EMIT this->removeIndexItem(path);
}

void DirWatcher::initData()
{
    //适配需求，可索引目录为用户指定。
//    m_indexableDirList << "/data" << QDir::homePath();
    /* boot里面存放Linux核心文件，开机选单与开机所需配置文件等
     * backup里面是系统备份文件
     * bin放置的是在单人维护模式下还能够被操作的指令，在bin底下的指令可以被root与一般账号所使用。
     * dev里面存放设备文件
     * etc里面存放了几乎所有的系统主要配置文件，包括人员的账号密码文件，各种服务的起始档等
     * lib放置最基本的共享库和内核模块，lib32，lib64，libx32分别面向32位，64位以及x32 ABI。他们都分别连接到usr下的lib*中
     * media一般放置可移除的装置，包括软盘，光盘和一些移动存储设备都自动挂载到这里
     * mnt原本和media用途相同，现用来放置暂时挂载一些额外装置
     * usr是Unix操作系统软件资源所放置的目录,所有系统默认的软件(distribution发布者提供的软件)都会放置到usr底下
     * var目录主要针对常态性变动的文件，包括缓存(cache)、登录档(log file)以及某些软件运作所产生的文件，包括程序文件(lock file, run file)，或者如MySQL数据库的文件等
     */
    m_blackListOfIndex << "/boot" << "backup" << "bin" << "/dev" << "/etc" << "/usr" << "/var"
                       << "/lib" << "lib32" << "lib64" << "libx32" << "/media" << "/mnt" << "cdrom"
                       << "/sys" << "/proc" << "tmp" << "/srv" << "/sbin" << "/run" << "/opt";

    //目前方案：可搜索目录（服务）默认根目录，可搜索目录（应用）默认家目录和/data目录
    m_searchableListForApplication << "/data" << QDir::homePath();
    m_searchableDirList << "/";

    //init auto mounted device list
    setfsent();
    while (1) {
        fstab *myFstab = getfsent();
        if (!myFstab) {
            endfsent();
            break;
        }
        QString automaticMountPoint = myFstab->fs_file;
        QString spec = myFstab->fs_spec;

        //目前只索引data和home，因此只存这两个文件夹下的挂载点
        if (automaticMountPoint.contains("/data") || automaticMountPoint.contains("/home")) {
            m_autoMountList.append(automaticMountPoint);
        }
        if (!spec.startsWith("UUID")) {
            m_infoOfSubvolume.insert(spec, automaticMountPoint);
        }
    }

    //init disk data, refresh the black list
    handleDisk();
}

void DirWatcher::initDiskWatcher()
{
    //use Dbus to monitor the hot plug of Udisk.


    QDBusConnection dbc = QDBusConnection::systemBus();

    qDebug() << dbc.connect("org.freedesktop.UDisks2",
                                                     "/org/freedesktop/UDisks2",
                                                     "org.freedesktop.DBus.ObjectManager",
                                                     "InterfacesAdded",
                                                     this, SLOT(handleAddedUDiskDevice(QDBusMessage)));
    qDebug() << dbc.lastError();
    qDebug() << dbc.connect("org.freedesktop.UDisks2",
                                                     "/org/freedesktop/UDisks2",
                                                     "org.freedesktop.DBus.ObjectManager",
                                                     "InterfacesRemoved",
                                                     this, SLOT(handleRemovedUDiskDevice(QDBusMessage)));

    m_volumeMonitor = g_volume_monitor_get();
    if (!m_volumeMonitor) {
        return;
    }
    m_mountAddHandle = g_signal_connect(m_volumeMonitor, "mount-added", G_CALLBACK(mountAddCallback), this);
    m_mountRemoveHandle = g_signal_connect(m_volumeMonitor, "mount-removed", G_CALLBACK(mountRemoveCallback), this);
}

void DirWatcher::handleDisk()
{
    //init current mounted device info
    QMutexLocker locker(&s_mutex);
    m_currentMountedDeviceInfo.clear();
    for (QStorageInfo &storage: QStorageInfo::mountedVolumes()) {
        if (storage.isValid() && storage.isReady() && QString(storage.device()).contains(QRegExp("/sd[a-z][1-9]"))) {
            m_currentMountedDeviceInfo[storage.device()].append(storage.rootPath());

            if (m_currentMountedDeviceInfo.value(storage.device()).length() > 1) {
                m_repeatedlyMountedDeviceInfo.insert(storage.device(), m_currentMountedDeviceInfo.value(storage.device()));
            }
            if (storage.rootPath().startsWith("/data") || storage.rootPath().startsWith("/home")) {
                m_blackListOfIndex.append(storage.rootPath());
            }
        }
    }

    //init udisk info
    if (!m_addedUDiskDeviceList.isEmpty()) {
        for (QString addedUDiskDevice: m_addedUDiskDeviceList) {
            m_currentUDiskDeviceInfo.insert(addedUDiskDevice, m_currentMountedDeviceInfo.value("/dev/" + addedUDiskDevice));
        }
    }

    //将u盘设备在/data和/home下的所有挂载点添加到索引黑名单
    if (!m_currentUDiskDeviceInfo.isEmpty()) {
        for (auto t = m_currentUDiskDeviceInfo.constBegin(); t != m_currentUDiskDeviceInfo.constEnd(); t++) {
            for (QString udiskDevice: t.value()) {
                if (udiskDevice.startsWith("/data") || udiskDevice.startsWith("/home")) {
                    m_blackListOfIndex.append(udiskDevice);
                }
            }
        }
    }

    //从黑名单中移除所有自动挂载设备（目前只包含自动挂载到/data和/home目录下的设备）
    for (QString autoMountDevice: m_autoMountList) {
        m_blackListOfIndex.removeAll(autoMountDevice);
    }
    m_blackListOfIndex.removeDuplicates();

    qDebug() << "autoMountList:" << m_autoMountList;
    qDebug() << "m_currentMountedDeviceInfo:" << m_currentMountedDeviceInfo;
    qDebug() << "m_repeatedlyMountedDeviceInfo:" << m_repeatedlyMountedDeviceInfo;
    qDebug() << "m_currentUDiskDeviceInfo:" << m_currentUDiskDeviceInfo;
    qDebug() << "m_blackListOfIndex:" << m_blackListOfIndex;

}

void DirWatcher::handleAddedUDiskDevice(QDBusMessage msg)
{
    QDBusObjectPath objPath = msg.arguments().at(0).value<QDBusObjectPath>();
    if (objPath.path().contains(QRegExp("/sd[a-z][1-9]"))) {
        QMutexLocker locker(&s_mutex);
        m_addedUDiskDeviceList.append(objPath.path().section("/",-1));
        qDebug() << "Add Udisk:" << m_addedUDiskDeviceList;
    }
}

void DirWatcher::handleRemovedUDiskDevice(QDBusMessage msg)
{
    Q_EMIT this->udiskRemoved();
    QDBusObjectPath objPath = msg.arguments().at(0).value<QDBusObjectPath>();
    if (objPath.path().contains(QRegExp("/sd[a-z][1-9]"))) {
        QMutexLocker locker(&s_mutex);
        m_removedUDiskDevice = objPath.path().section("/",-1);
        m_addedUDiskDeviceList.removeAll(m_removedUDiskDevice);
    }
}
