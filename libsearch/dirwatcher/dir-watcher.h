#ifndef MOUNTDISKLISTENER_H
#define MOUNTDISKLISTENER_H

#include "dir-watcher-adaptor.h"

#include <QObject>
#include <QStorageInfo>
#include <QHash>
#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QMutex>
#include <QSettings>
#include <QDBusConnection>

#undef slots
#undef signals
#undef emit

#include <gio/gio.h>
#include <gio/gunixmounts.h>

class DirWatcher : public QObject
{
    Q_OBJECT

    Q_CLASSINFO("D-Bus Interface","org.ukui.search.fileindex")

public:
    static DirWatcher *getDirWatcher();
    void initDbusService();
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
    void appendIndexableListItem(const QString &path);
    void removeIndexableListItem(const QString &path);

    void handleDisk();
    void handleAddedUDiskDevice(QDBusMessage msg);
    void handleRemovedUDiskDevice(QDBusMessage msg);
private:
    DirWatcher(QObject *parent = nullptr);
    ~DirWatcher();
    void initData();
    void initDiskWatcher();

    void handleIndexItemAppend(const QString &path, QStringList &blackList);
    void handleIndexItemRemove(const QString &path);

    static QMutex s_mutex;

    GVolumeMonitor *m_volumeMonitor = nullptr;

    DirWatcherAdaptor *m_adaptor = nullptr;

    quint64 m_mountAddHandle;
    quint64 m_mountRemoveHandle;

    QSettings *m_qSettings = nullptr;
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
    void appendIndexItem(const QString&, const QStringList&);
    void removeIndexItem(const QString&);
};

#endif // MOUNTDISKLISTENER_H

