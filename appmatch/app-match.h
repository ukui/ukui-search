#ifndef APPMATCH_H
#define APPMATCH_H

#include <QObject>
#include <QDir>
#include <QLocale>
#include <QDebug>
class AppMatch : public QObject
{
    Q_OBJECT
public:
    explicit AppMatch(QObject *parent = nullptr);
    QStringList startMatchApp(QString input);

private:
    void getAllDesktopFilePath(QString path);
    void getDesktopFilePath();
    void getAppName();
    void appNameMatch(QString appname,QString desktoppath);

private:
    QString m_soureText;
    QStringList m_filePathList;
    QStringList m_returnResult;
    QStringList m_midResult;

};

#endif // APPMATCH_H
