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
    static void writeConfig(QString message);
    static QMap<QString,QStringList> readConfig();
    static void receiveMessage(QString message);
private:
   static void writeCommonly(QString message);
   static QStringList readCommonly();
   static void writeRecently(QString message);
   static QStringList readRecently();

};

#endif // CONFIGFILE_H
