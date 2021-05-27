#ifndef FILESEARCHPLUGIN_H
#define FILESEARCHPLUGIN_H

#include <QObject>
#include <QThreadPool>

#include "libsearch_global.h"
#include "search-plugin-iface.h"
#include "common.h"
namespace Zeeker {
//internal plugin
class LIBSEARCH_EXPORT FileSearchPlugin : public QObject, public SearchPluginIface
{
    Q_OBJECT
public:
    FileSearchPlugin(QObject *parent = nullptr);
    PluginType pluginType() {return PluginType::SearchPlugin;}
    const QString name();
    const QString description();
    const QIcon icon() {return QIcon::fromTheme("folder");}
    void setEnable(bool enable) {m_enable = enable;}
    bool isEnable() {return m_enable;}
    QString getPluginName();

    void KeywordSearch(QString keyword,DataQueue<ResultInfo> *searchResult);
    QList<SearchPluginIface::Actioninfo> getActioninfo(int type);
    void openAction(int actionkey, QString key);

private:
    bool m_enable = true;
    QList<SearchPluginIface::Actioninfo> m_actionInfo;
    QThreadPool m_pool;
};

class LIBSEARCH_EXPORT DirSearchPlugin : public QObject, public SearchPluginIface
{
    Q_OBJECT
public:
    DirSearchPlugin(QObject *parent = nullptr);
    PluginType pluginType() {return PluginType::SearchPlugin;}
    const QString name();
    const QString description();
    const QIcon icon() {return QIcon::fromTheme("folder");}
    void setEnable(bool enable) {m_enable = enable;}
    bool isEnable() {return m_enable;}
    QString getPluginName();

    void KeywordSearch(QString keyword,DataQueue<ResultInfo> *searchResult);
    QList<SearchPluginIface::Actioninfo> getActioninfo(int type);
    void openAction(int actionkey, QString key);

private:
    bool m_enable = true;
    QList<SearchPluginIface::Actioninfo> m_actionInfo;
    QThreadPool m_pool;
};

class LIBSEARCH_EXPORT FileContengSearchPlugin : public QObject, public SearchPluginIface
{
    Q_OBJECT
public:
    FileContengSearchPlugin(QObject *parent = nullptr);
    PluginType pluginType() {return PluginType::SearchPlugin;}
    const QString name();
    const QString description();
    const QIcon icon() {return QIcon::fromTheme("folder");}
    void setEnable(bool enable) {m_enable = enable;}
    bool isEnable() {return m_enable;}
    QString getPluginName();

    void KeywordSearch(QString keyword,DataQueue<ResultInfo> *searchResult);
    QList<SearchPluginIface::Actioninfo> getActioninfo(int type);
    void openAction(int actionkey, QString key);

private:
    bool m_enable = true;
    QList<SearchPluginIface::Actioninfo> m_actionInfo;
    QThreadPool m_pool;
};
}


#endif // FILESEARCHPLUGIN_H
