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
#define DEFAULT_INDEXABLE_DIR QDir::homePath()

static std::once_flag flag;
static DirWatcher *global_intance = nullptr;
QMutex DirWatcher::s_mutex;

DirWatcher::DirWatcher(QObject *parent) : QObject(parent)
{
    m_qSettings = new QSettings(CURRENT_INDEXABLE_DIR_SETTINGS, QSettings::IniFormat);
    this->currentIndexableDir();
    if (m_indexableDirList.isEmpty()) {
        qDebug() << QString("use the path: %1 as default indexable dir.").arg(DEFAULT_INDEXABLE_DIR);
        m_qSettings->beginGroup(INDEXABLE_DIR_VALUE);
        m_qSettings->setValue(INDEXABLE_DIR_VALUE, DEFAULT_INDEXABLE_DIR);
        m_qSettings->endGroup();
    }

    initDiskWatcher();
    initData();
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
    //排除要添加的路径已被索引的情况
    if (m_indexableDirList.contains(path)) {
        qDebug() << QString("index path %1 is already added.").arg(path);
        return;
    }

    //处理添加路径非根目录时，要添加索引的路径与已索引路径为父子关系的情况
    if (path != "/") {
        for (int i = 0; i < m_indexableDirList.length(); i++) {
            const QString indexablePath = m_indexableDirList.at(i);
            if (path.startsWith(indexablePath + "/")) {
                qCritical() << QString("The parent of the path:%1 has been added.").arg(path);
                return;
            }
            if (indexablePath.startsWith(path + "/")) {
                m_indexableDirList.removeAll(indexablePath);
                blackList.append(indexablePath);
            }
        }
    }

    m_indexableDirList << path;
    m_qSettings->beginGroup(INDEXABLE_DIR_VALUE);
    m_qSettings->setValue(INDEXABLE_DIR_VALUE, m_indexableDirList);
    m_qSettings->endGroup();
    blackList.removeDuplicates();
    Q_EMIT this->appendIndexItem(path, blackList);
    qDebug() << "index path:" << path << "blacklist:" << blackList;
}

void DirWatcher::handleIndexItemRemove(const QString &path)
{
    this->currentIndexableDir();
    QMutexLocker locker(&s_mutex);
    if (!m_indexableDirList.contains(path)) {
        qWarning() << QString("The path: %1 is not indexed").arg(path);
        return;
    }
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
    GMount* mount = (GMount*)g_object_ref(gmount);
    GVolume* volume = g_mount_get_volume(mount);
    if (volume) {
        bool canEject = g_volume_can_eject(volume);
        QString devName = g_volume_get_identifier(volume, G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
        if (canEject and devName.contains(QRegExp("/sd[a-z][1-9]"))) {
            QMutexLocker locker(&s_mutex);
            pThis->m_currentUDiskDeviceList.append(devName.section("/", -1));
        }
        qDebug() << "added device name:" << devName.section("/", -1);
        g_object_unref(volume);
    } else {
        qWarning() << "GVolume(add) is NULL.";
    }
    pThis->handleDisk();
}

void DirWatcher::mountRemoveCallback(GVolumeMonitor *monitor, GMount *gmount, DirWatcher *pThis)
{
    qDebug() << "Mount Removed";
    pThis->handleDisk();
    QMutexLocker locker(&s_mutex);
    //处理u盘设备(由于udisk2信号连不上，不生效）
    if (pThis->m_removedUDiskDevice != NULL) {
        pThis->m_currentUDiskDeviceInfo.remove(pThis->m_removedUDiskDevice);
        qDebug() << "m_currentUDiskDeviceInfo(after remove):" << pThis->m_currentUDiskDeviceInfo;
        pThis->m_removedUDiskDevice = "";
        return;
    }

    GMount* mount = (GMount*)g_object_ref(gmount);
    GFile* rootFile;
    rootFile = g_mount_get_root(mount);
    if (!rootFile) {
        return;
    }
    QString removedUri = g_file_get_uri(rootFile);
    if (removedUri.isEmpty()) {
        return;
    }
    //处理uri转码
    if (removedUri.startsWith("file:///")) {
        QString removedMountPoint = g_filename_from_uri(removedUri.toUtf8().constData(), nullptr, nullptr);
        pThis->m_blackListOfIndex.removeAll(removedMountPoint);
        QStringList indexableDirList = pThis->currentIndexableDir();
        //卸载目录下存在已索引目录时，通知索引服务删除对应目录
        for (const QString &indexableDir : indexableDirList) {
            if (indexableDir.startsWith(removedMountPoint + "/") or !indexableDir.compare(removedMountPoint)) {
                Q_EMIT pThis->removeIndexItem(indexableDir);
            }
        }
        //处理子卷情况
        for (auto t = pThis->m_infoOfSubvolume.constBegin(); t != pThis->m_infoOfSubvolume.constEnd(); t++) {
            if (removedMountPoint.startsWith(t.value() + "/")) {
                pThis->m_blackListOfIndex.removeAll(removedMountPoint.replace(t.value(), t.key()));
            }
            if (removedMountPoint.startsWith(t.key() + "/")) {
                pThis->m_blackListOfIndex.removeAll(removedMountPoint.replace(t.key(), t.value()));
            }
        }
        qDebug() << "m_blackListOfIndex(after remove):" << pThis->m_blackListOfIndex;
    } else {
        qWarning() << QString("Removed uri:%1 is not starts with 'file:///', there's no handling of it.").arg(removedUri);
    }
    g_object_unref(rootFile);
}

void DirWatcher::appendIndexableListItem(const QString &path)
{
    //排除path不存在的情况
    QFile file(path);
    if (!file.exists()) {
        qWarning() << QString("target path:%1 is not exists!").arg(path);
        return;
    }

    //同步配置文件中的已索引目录
    this->currentIndexableDir();
    qDebug() << "current indexable dirs:" << m_indexableDirList;

    QStringList blackList;
    QMutexLocker locker(&s_mutex);

    //根目录特殊处理
    if (path == "/") {
        this->handleIndexItemAppend(path, m_blackListOfIndex);
        return;
    }

    //处理要添加索引的路径与索引黑名单中路径为父子关系的情况
    for (const QString& blackListPath : m_blackListOfIndex) {
        if (path.startsWith(blackListPath + "/") or path == blackListPath) {
            qCritical() << QString("path:%1 is or under the blacklistpath:%2.").arg(path, blackListPath);
            return;
        }

        if (blackListPath.startsWith(path + "/")) {
            blackList.append(blackListPath);
        }
    }

    //排除要添加的目录为某设备的重复挂载目录，并且之前已索引过该设备其他挂载目录或其父目录的情况
    for (auto i = m_repeatedlyMountedDeviceInfo.constBegin(); i != m_repeatedlyMountedDeviceInfo.constEnd(); i++) {
        bool pathToBeAddedIsRepeatedDevice = false;
        bool pathToBeAddedHasRepeatedDevice = false;
        bool addedPathIsRepeatedDevice = false;
        bool addedPathHasRepeatedDevice = false;
        QString addedRelativeDir;
        QString repeatedDir;
        for (const QString &addedPath : m_indexableDirList) {
            for (const QString &mountPoint : i.value()) {

                //要添加索引路径在重复挂载设备路径下(1)
                if (path.startsWith(mountPoint + "/") or mountPoint == path) {
                    repeatedDir = mountPoint;
                    pathToBeAddedIsRepeatedDevice = true;
                }
                //重复挂载设备路径在要添加索引路径下(2)
                if (mountPoint.startsWith(path + "/")) {
                    repeatedDir = mountPoint;
                    pathToBeAddedHasRepeatedDevice = true;
                }

                //已索引路径在重复挂载设备路径下(3)
                if (addedPath.startsWith(mountPoint + "/") or mountPoint == addedPath) {
                    addedRelativeDir = addedPath;
                    addedRelativeDir.remove(mountPoint);
                    addedPathIsRepeatedDevice = true;
                }
                //重复挂载设备路径在已索引路径下(4)
                if (mountPoint.startsWith(addedPath + "/")) {
                    addedPathHasRepeatedDevice = true;
                }

                //(1)(4)直接返回
                if (pathToBeAddedIsRepeatedDevice and addedPathHasRepeatedDevice) {
                    qCritical() << "current path is in repeat mounted devices and another path which is in the same device has been indexed!";
                    return;
                }
                //(2)(4)将要添加索引目录相应的重复挂载路径添加到黑名单
                if (pathToBeAddedHasRepeatedDevice and addedPathHasRepeatedDevice) {
                    blackList.append(repeatedDir);
                    break;
                }
                //(1)(3)将已索引路径的前缀替换为要添加路径的前缀（前缀为mountPoint），判断替换后路径是否在要索引路径下，如果是则返回，否则将替换后路径添加到黑名单
                if (pathToBeAddedIsRepeatedDevice and addedPathIsRepeatedDevice) {
                    QString pathAfterReplace = repeatedDir + addedRelativeDir;
                    if (path.startsWith(pathAfterReplace) or path == pathAfterReplace) {
                        qCritical() << QString("another path:%1 which is in the same device has been indexed").arg(pathAfterReplace);
                        return;
                    } else {
                        blackList.append(pathAfterReplace);
                        break;
                    }
                }
                //(2)(3)将替换前缀后的已索引路径添加到黑名单
                if (pathToBeAddedHasRepeatedDevice and addedPathIsRepeatedDevice) {
                    blackList.append(repeatedDir + addedRelativeDir);
                    break;
                }
            }
        }
    }

    //排除重复挂载设备的目录
    for (auto i = m_repeatedlyMountedDeviceInfo.constBegin(); i != m_repeatedlyMountedDeviceInfo.constEnd(); i++) {
        QString topRepeatedMountPoint;
        for (const QString &mountPoint : i.value()) {
            if (mountPoint.startsWith(path + "/") or mountPoint == path) {
                if (topRepeatedMountPoint.isEmpty()) {
                    topRepeatedMountPoint = mountPoint;
                    continue;
                } else if (topRepeatedMountPoint.startsWith(mountPoint)) {
                    blackList.append(topRepeatedMountPoint);
                    topRepeatedMountPoint = mountPoint;
                } else {
                    blackList.append(mountPoint);
                }
            }
        }
    }

    //处理自动挂载子卷下的目录，目前方案将其合并为一个，只保留了mountpoint下的目录
    for (auto t = m_infoOfSubvolume.constBegin(); t != m_infoOfSubvolume.constEnd(); t++) {
        QString mountPoint = t.value();
        QString spec = t.key();
        QString tmp = path;

        if (spec.startsWith(path + "/")) {
            blackList << spec;
        }

        if (path.startsWith(spec + "/") or path == spec) {
            tmp.replace(0, spec.length(), mountPoint);
            this->handleIndexItemAppend(tmp, blackList);
            qDebug() << QString("The path:%1 has been replaced into %2").arg(path, tmp);
            return;
        }
    }

    this->handleIndexItemAppend(path, blackList);
}

void DirWatcher::removeIndexableListItem(const QString &path)
{
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
    //将磁盘分区后其他分区都会挂载到media下，多块硬盘也会挂到media，因此media放开，mnt同理；
    //backup是备份文件，tmp是临时文件，也都放开
    m_blackListOfIndex << "/boot" << "/bin" << "/dev" << "/etc" << "/usr" << "/var"
                       << "/lib" << "/lib32" << "/lib64" << "/libx32" << "/cdrom"
                       << "/sys" << "/proc" << "/srv" << "/sbin" << "/run" << "/opt";

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
        //存储所有子卷自动挂载
        if (!spec.startsWith("UUID")) {
            m_infoOfSubvolume.insert(spec, automaticMountPoint);
        }
    }

    GList *list = g_volume_monitor_get_volumes(m_volumeMonitor);
    if (!list) {
        qDebug() << "Fail to init glist of volume monitor!";
        handleDisk();
        return;
    }
    for (guint i = 0; i < g_list_length(list); i++) {
        GVolume *volume = (GVolume*)g_list_nth_data(list, i);
        QString udiskDevName = g_volume_get_identifier(volume, G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
        bool canEject = g_volume_can_eject(volume);
        if (canEject and udiskDevName.contains(QRegExp("/sd[a-z][1-9]"))) {
            qDebug() << "udiskDevName:" << udiskDevName.section("/",-1);
            m_currentUDiskDeviceList.append(udiskDevName.section("/",-1));
        }
    }

    //init disk data, refresh the black list
    handleDisk();
}

void DirWatcher::initDiskWatcher()
{
    //use Dbus to monitor the hot plug of Udisk.


    QDBusConnection dbc = QDBusConnection::systemBus();

    qDebug() << "Connect The Signal:InterfacesAdded" << dbc.connect("org.freedesktop.UDisks2",
                                                                     "/org/freedesktop/UDisks2",
                                                                     "org.freedesktop.DBus.ObjectManager",
                                                                     "InterfacesAdded",
                                                                     this, SLOT(handleAddedUDiskDevice(QDBusMessage)));
    qDebug() << dbc.lastError();
    qDebug() << "Connect The Signal:InterfacesRemove" << dbc.connect("org.freedesktop.UDisks2",
                                                                      "/org/freedesktop/UDisks2",
                                                                      "org.freedesktop.DBus.ObjectManager",
                                                                      "InterfacesRemoved",
                                                                      this, SLOT(handleRemovedUDiskDevice(QDBusMessage)));

    m_volumeMonitor = g_volume_monitor_get();
    if (!m_volumeMonitor) {
        qDebug() << "Fail to init volume monitor";
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
    m_repeatedlyMountedDeviceInfo.clear();
    for (QStorageInfo &storage: QStorageInfo::mountedVolumes()) {
        //遍历当前系统所有挂载的，且以sd*开头的存储设备
        if (storage.isValid() and storage.isReady() and QString(storage.device()).contains(QRegExp("/sd[a-z][1-9]"))) {
            m_currentMountedDeviceInfo[storage.device()].append(storage.rootPath());
            //存储非子卷的重复挂载设备
            if (m_currentMountedDeviceInfo.value(storage.device()).length() > 1 and storage.subvolume().isEmpty()) {
                m_repeatedlyMountedDeviceInfo.insert(storage.device(), m_currentMountedDeviceInfo.value(storage.device()));
            }
            //排除挂载到data和home下挂载的所有其他设备，后面需要修改
//            if (storage.rootPath().startsWith("/data") || storage.rootPath().startsWith("/home")) {
//                m_blackListOfIndex.append(storage.rootPath());
//            }
        }
    }

    //根据设备号(key)更新u盘信息
    if (!m_currentUDiskDeviceList.isEmpty()) {
        for (const QString &udiskDevice: m_currentUDiskDeviceList) {
            QStringList udiskMountPointList = m_currentMountedDeviceInfo.value("/dev/" + udiskDevice);
            if (udiskMountPointList.isEmpty()) {
                m_currentUDiskDeviceInfo.remove(udiskDevice);
            } else {
                m_currentUDiskDeviceInfo.insert(udiskDevice, udiskMountPointList);
            }

        }
    }

    //将u盘设备在/data和/home下的所有挂载点添加到索引黑名单, 目前由于未知原因收不到udisk挂载信号因此暂时不生效
    if (!m_currentUDiskDeviceInfo.isEmpty()) {
        for (auto t = m_currentUDiskDeviceInfo.constBegin(); t != m_currentUDiskDeviceInfo.constEnd(); t++) {
            for (QString udiskDevice: t.value()) {
                if (udiskDevice.startsWith("/data") || udiskDevice.startsWith("/home")) {
                    m_blackListOfIndex.append(udiskDevice);
                }
            }
        }
    }

    //从索引黑名单中移除所有自动挂载设备（目前只包含自动挂载到/data和/home目录下的设备），m_infoOfSubvolume存储全部fstab文件中的子卷自动挂载
    for (const QString &autoMountDevice: m_autoMountList) {
        m_blackListOfIndex.removeAll(autoMountDevice);
    }
    m_blackListOfIndex.removeDuplicates();

    qDebug() << "m_infoOfSubvolume" << m_infoOfSubvolume;
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
        m_currentUDiskDeviceList.append(objPath.path().section("/",-1));
        qDebug() << "Add Udisk:" << m_currentUDiskDeviceList;
    }
}

void DirWatcher::handleRemovedUDiskDevice(QDBusMessage msg)
{
    Q_EMIT this->udiskRemoved();
    QDBusObjectPath objPath = msg.arguments().at(0).value<QDBusObjectPath>();
    if (objPath.path().contains(QRegExp("/sd[a-z][1-9]"))) {
        QMutexLocker locker(&s_mutex);
        m_removedUDiskDevice = objPath.path().section("/",-1);
        m_currentUDiskDeviceList.removeAll(m_removedUDiskDevice);
    }
}