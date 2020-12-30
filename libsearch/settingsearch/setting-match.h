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
    void xmlElement();
    QStringList matching();

private:
    QMap<QString,QStringList> m_pinyin_searchList;
    QMap<QString,QStringList> m_chine_searchList;
    QStringList m_pinyin_searchResult;
    QStringList m_chine_searchResult;
    QString m_sourceText;

};

#endif // SETTINGSEARCH_H
