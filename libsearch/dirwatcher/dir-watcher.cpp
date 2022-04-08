#include "dir-watcher.h"

#include <QDebug>
#include <QDir>
#include <QDBusConnection>
#include <QDBusArgument>
#include <QThread>
#include <fstab.h>
#include <QMutexLocker>


static std::once_flag flag;
static DirWatcher *global_intance = nullptr;
QMutex DirWatcher::s_mutex;

DirWatcher::DirWatcher(QObject *parent) : QObject(parent)
{
    initData();
    initDiskWatcher();
}

DirWatcher::~DirWatcher()
{
    if (m_volumeMonitor) {
        g_signal_handler_disconnect(m_volumeMonitor, m_mountAddHandle);
        g_signal_handler_disconnect(m_volumeMonitor, m_mountRemoveHandle);
        m_volumeMonitor = nullptr;
    }
}

DirWatcher *DirWatcher::getDirWatcher()
{
    std::call_once(flag, [ & ] {
        global_intance = new DirWatcher();
    });
    return global_intance;
}

QStringList DirWatcher::currentindexableDir()
{
    QMutexLocker locker(&s_mutex);
    QStringList indexableDirList = m_indexableDirList;
    return indexableDirList;
}

QStringList DirWatcher::currentBlackListOfIndex()
{
    QMutexLocker locker(&s_mutex);
    QStringList blackListOfIndex = m_blackListOfIndex;
    return blackListOfIndex;
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

void DirWatcher::initData()
{
    //目前方案只索引数据盘和家目录。
    m_indexableDirList << "/data" << QDir::homePath();
    m_blackListOfIndex << "/data/home" << "/data/root";

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
    QDBusConnection::systemBus().connect("org.freedesktop.UDisks2",
                                         "/org/freedesktop/UDisks2",
                                         "org.freedesktop.DBus.ObjectManager",
                                         "InterfacesAdded",
                                         this, SLOT(handleAddedUDiskDevice(QDBusMessage)));
    QDBusConnection::systemBus().connect("org.freedesktop.UDisks2",
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
