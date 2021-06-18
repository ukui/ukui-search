#ifndef SETTINGSSEARCHPLUGIN_H
#define SETTINGSSEARCHPLUGIN_H

#include <QObject>
#include <QThreadPool>
#include "search-plugin-iface.h"

namespace Zeeker {
class LIBSEARCH_EXPORT SettingsSearchPlugin : public QObject, public SearchPluginIface
{
    Q_OBJECT
public:
    SettingsSearchPlugin(QObject *parent = nullptr);
    PluginType pluginType() {return PluginType::SearchPlugin;}
    const QString name();
    const QString description();
    const QIcon icon() {return QIcon::fromTheme("folder");}
    void setEnable(bool enable) {m_enable = enable;}
    bool isEnable() {return m_enable;}
    QString getPluginName();

    void KeywordSearch(QString keyword,DataQueue<ResultInfo> *searchResult);
    QList<SearchPluginIface::Actioninfo> getActioninfo(int type);
    void openAction(int actionkey, QString key, int type);

private:
    void xmlElement();

    QMap<QString, QStringList> m_chineseSearchList;
    QMap<QString, QStringList> m_englishSearchList;

    bool m_enable = true;
    QList<SettingsSearchPlugin::Actioninfo> m_actionInfo;
    QThreadPool m_pool;
};
}
#endif // SETTINGSSEARCHPLUGIN_H
