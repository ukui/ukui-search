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
#include <QDBusInterface>
#include <QDBusReply>

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
    static void mountAddCallback(GVolumeMonitor *monitor, GMount *gmount, DirWatcher *pThis);
    static void mountRemoveCallback(GVolumeMonitor *monitor, GMount *gmount, DirWatcher *pThis);

    void appendBlackListItemOfIndex(const QString &path);
    void appendBlackListItemOfIndex(const QStringList &pathList);
    void removeBlackListItemOfIndex(const QString &path);
    void removeBlackListItemOfIndex(const QStringList &pathList);

public Q_SLOTS:

    void initDbusService();

    QStringList currentIndexableDir();
    QStringList currentBlackListOfIndex();

    QStringList currentSearchableDir();
    QStringList searchableDirForSearchApplication();
    QStringList blackListOfDir(const QString &dirPath);

    Q_SCRIPTABLE void appendIndexableListItem(const QString &path);
    Q_SCRIPTABLE void removeIndexableListItem(const QString &path);

private:
    DirWatcher(QObject *parent = nullptr);
    ~DirWatcher();
    void initData();
    void initDiskWatcher();

    void handleDisk();
    void handleAddedUDiskDevice(QDBusMessage msg);
    void handleRemovedUDiskDevice(QDBusMessage msg);

    void handleIndexItemAppend(const QString &path, QStringList &blackList);
    void handleIndexItemRemove(const QString &path);

    static QMutex s_mutex;

    DirWatcherAdaptor *m_adaptor = nullptr;

    GVolumeMonitor *m_volumeMonitor = nullptr;
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

    QStringList m_currentUDiskDeviceList;
    QString m_removedUDiskDevice;
    QMap<QString, QStringList> m_currentUDiskDeviceInfo;
Q_SIGNALS:
    void udiskRemoved();
    void appendIndexItem(const QString&, const QStringList&);
    void removeIndexItem(const QString&);
};

#endif // MOUNTDISKLISTENER_H

