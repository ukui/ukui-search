#ifndef SETTINGSSEARCHPLUGIN_H
#define SETTINGSSEARCHPLUGIN_H

#include <QObject>
#include <QThreadPool>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QAction>
#include "action-label.h"
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
//    bool isPreviewEnable(QString key, int type);
//    QWidget *previewPage(QString key, int type, QWidget *parent);
    QWidget *detailPage(const ResultInfo &ri);

private:
    void xmlElement();
    void initDetailPage();
    QString m_currentActionKey;
    QWidget *m_detailPage = nullptr;
    QVBoxLayout *m_detailLyt = nullptr;
    QLabel *m_iconLabel = nullptr;
    QFrame *m_nameFrame = nullptr;
    QHBoxLayout *m_nameFrameLyt = nullptr;
    QLabel *m_nameLabel = nullptr;
    QLabel *m_pluginLabel = nullptr;
    QFrame *m_line_1 = nullptr;
    QFrame *m_actionFrame = nullptr;
    QVBoxLayout *m_actionFrameLyt = nullptr;
    ActionLabel *m_actionLabel1 = nullptr;

    QVBoxLayout * m_actionLyt = nullptr;


    QMap<QString, QStringList> m_chineseSearchList;
    QMap<QString, QStringList> m_englishSearchList;

    bool m_enable = true;
    QList<SettingsSearchPlugin::Actioninfo> m_actionInfo;
    QThreadPool m_pool;
};
}
#endif // SETTINGSSEARCHPLUGIN_H
