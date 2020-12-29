#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <QObject>
#include <QSettings>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
class ConfigFile : public QObject
{
    Q_OBJECT
public:
    explicit ConfigFile(QObject *parent = nullptr);
    void writeConfig();
    QMap<QString,QStringList> readConfig();

private:
    QSettings *m_qSettings;
    QString m_message;

private:
    void writeCommonly(QString message);
    QStringList readCommonly();
    void writeRecently(QString message);
    QStringList readRecently();


public Q_SLOTS:
    void receiveMessage(QString message);

Q_SIGNALS:

};

#endif // CONFIGFILE_H
