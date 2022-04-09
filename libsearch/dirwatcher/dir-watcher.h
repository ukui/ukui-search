#ifndef MOUNTDISKLISTENER_H
#define MOUNTDISKLISTENER_H

#include <QObject>
#include <QStorageInfo>
#include <QHash>
#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QMutex>

#undef slots
#undef signals
#undef emit

#include <gio/gio.h>
#include <gio/gunixmounts.h>

class DirWatcher : public QObject
{
    Q_OBJECT
public:
    static DirWatcher *getDirWatcher();
    QStringList currentindexableDir();
    QStringList currentBlackListOfIndex();
    void appendBlackListItemOfIndex(const QString &path);
    void appendBlackListItemOfIndex(const QStringList &pathList);
    void removeBlackListItemOfIndex(const QString &path);
    void removeBlackListItemOfIndex(const QStringList &pathList);

    QStringList currentSearchableDir();
    QStringList searchableDirForSearchApplication();
    QStringList blackListOfDir(const QString &dirPath);

    static void mountAddCallback(GVolumeMonitor *monitor, GMount *gmount, DirWatcher *pThis);
    static void mountRemoveCallback(GVolumeMonitor *monitor, GMount *gmount, DirWatcher *pThis);
public Q_SLOTS:
    void handleDisk();
    void handleAddedUDiskDevice(QDBusMessage msg);
    void handleRemovedUDiskDevice(QDBusMessage msg);
private:
    DirWatcher(QObject *parent = nullptr);
    ~DirWatcher();
    void initData();
    void initDiskWatcher();

    static QMutex s_mutex;

    GVolumeMonitor *m_volumeMonitor = nullptr;
    quint64 m_mountAddHandle;
    quint64 m_mountRemoveHandle;

    QStringList m_blackListOfIndex;
    QStringList m_indexableDirList;
    QStringList m_searchableDirList;
    QStringList m_searchableListForApplication;
    QStringList m_autoMountList;
    QMultiMap<QString, QString> m_infoOfSubvolume;
    QMap<QString, QStringList> m_currentMountedDeviceInfo;
    QMap<QString, QStringList> m_repeatedlyMountedDeviceInfo;

    QStringList m_addedUDiskDeviceList;
    QString m_removedUDiskDevice;
    QMap<QString, QStringList> m_currentUDiskDeviceInfo;
Q_SIGNALS:
    void udiskRemoved();
};

#endif // MOUNTDISKLISTENER_H

