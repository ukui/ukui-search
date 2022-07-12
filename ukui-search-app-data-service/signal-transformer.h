#ifndef SIGNALTRANSFORMER_H
#define SIGNALTRANSFORMER_H

#include <QObject>
#include <QDBusInterface>
#include <QMutexLocker>
#include "app-db-manager.h"

using namespace UkuiSearch;

class SignalTransformer : public QObject
{
    Q_CLASSINFO("D-Bus Interface","org.ukui.search.signalTransformer")

    Q_OBJECT

public:
    static SignalTransformer *getTransformer();

    static QMutex s_mutex;

public Q_SLOTS:
    void handleItemInsert(const AppInfoResult &item);
    void handleItemUpdate(const AppInfoResult &item);
    void handleItemDelete(const QString &desktopfp);
    void handleSignalTransform();

private:
    SignalTransformer(QObject *parent = nullptr);
    SignalTransformer(const SignalTransformer &) = delete;
    SignalTransformer& operator = (const SignalTransformer&) = delete;

    QVector<AppInfoResult> m_items2BUpdate;
    QVector<AppInfoResult> m_items2BInsert;
    QStringList m_items2BDelete;

Q_SIGNALS:
    void appDBItemsUpdate(QVector<AppInfoResult>);
    void appDBItemsAdd(QVector<AppInfoResult>);
    void appDBItemsDelete(QStringList);
};

#endif // SIGNALTRANSFORMER_H
