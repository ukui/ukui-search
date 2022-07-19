#ifndef PENDINGAPPINFOQUEUE_H
#define PENDINGAPPINFOQUEUE_H

#include "pending-app-info.h"

#include <QThread>
#include <QMutexLocker>
#include <QTimer>
#include <QVector>

namespace UkuiSearch{

class PendingAppInfoQueue : public QThread
{
    Q_OBJECT
public:
    static PendingAppInfoQueue &getAppInfoQueue();
    void enqueue(const PendingAppInfo &appInfo);

protected:
    void run() override;

private:
    explicit PendingAppInfoQueue(QObject *parent = nullptr);
    PendingAppInfoQueue(const PendingAppInfoQueue&) = delete;
    PendingAppInfoQueue& operator =(const PendingAppInfoQueue&) = delete;
    ~PendingAppInfoQueue();

    void processCache();

private:
    static QMutex s_mutex;

    QTimer *m_minProcessTimer = nullptr;
    QTimer *m_maxProcessTimer = nullptr;

    QVector<UkuiSearch::PendingAppInfo> m_cache;
    QVector<UkuiSearch::PendingAppInfo> m_pendingAppInfos;

    int m_handleTimes;

Q_SIGNALS:
    void startMinProcessTimer();
    void startMaxProcessTimer();
    void stopTimer();
};
}



#endif // PENDINGAPPINFOQUEUE_H
