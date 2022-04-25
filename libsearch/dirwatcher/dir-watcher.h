#ifndef MOUNTDISKLISTENER_H
#define MOUNTDISKLISTENER_H

#include <QObject>
#include <QDBusInterface>
#include <QDBusReply>


class DirWatcher : public QObject
{
    Q_OBJECT
public:
    static DirWatcher *getDirWatcher();

public Q_SLOTS:
    void initDbusService();

    QStringList currentIndexableDir();
    QStringList currentBlackListOfIndex();

    QStringList currentSearchableDir();
    QStringList searchableDirForSearchApplication();
    QStringList blackListOfDir(const QString &dirPath);

    void appendIndexableListItem(const QString &path);
    void removeIndexableListItem(const QString &path);

private:
    DirWatcher(QObject *parent = nullptr);
    ~DirWatcher() = default;

    QDBusInterface *m_dbusInterface = nullptr;
};

#endif // MOUNTDISKLISTENER_H

