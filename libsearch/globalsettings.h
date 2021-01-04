#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#include <QObject>
#include <QSettings>
#include <QMutex>
#include <QVector>

class GlobalSettings : public QObject
{
    Q_OBJECT
public:
    static GlobalSettings *getInstance();
    const QVariant getValue(const QString&);
    bool isExist(const QString&);

Q_SIGNALS:
    void valueChanged (const QString&);

public Q_SLOTS:
    void setValue(const QString&, const QVariant&);
    void reset(const QString&);
    void resetAll();
    QList<QString> getBlockDirs();

    void forceSync(const QString& = nullptr);

private:
    explicit GlobalSettings(QObject *parent = nullptr);
    ~GlobalSettings();

    QSettings* m_settings;
    QMap<QString, QVariant> m_cache;

    QMutex m_mutex;



};

#endif // GLOBALSETTINGS_H
