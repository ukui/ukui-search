#ifndef SETTINGSEARCH_H
#define SETTINGSEARCH_H

#include <QObject>
#include <QFile>
#include <QDomElement>
#include <QDomDocument>
#include <QDomNode>
#include <QDomNodeList>
#include <QMap>
#include <QStringList>
#include <QTimer>
#include <QDebug>
class SettingsMatch : public QObject
{
    Q_OBJECT
public:
    explicit SettingsMatch(QObject *parent = nullptr);
    QStringList startMatchApp(const QString &source);

private:
    void XmlElement();
    QStringList matching();

private:
    QMap<QString,QStringList> m_Pinyin_searchList;
    QMap<QString,QStringList> m_Chine_searchList;
    QStringList m_Pinyin_searchResult;
    QStringList m_Chine_searchResult;
    QString m_sourceText;

};

#endif // SETTINGSEARCH_H
