#ifndef WEBSEARCHPLUGIN_H
#define WEBSEARCHPLUGIN_H

#include <QObject>
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

namespace UkuiSearch {
class LIBSEARCH_EXPORT WebSearchPlugin : public QObject, public SearchPluginIface
{
    Q_OBJECT

public:
    explicit WebSearchPlugin(QObject *parent = nullptr);
    PluginType pluginType() {return PluginType::SearchPlugin;}
    const QString name();
    const QString description();
    const QIcon icon() {return QIcon::fromTheme("folder");}
    void setEnable(bool enable) {m_enable = enable;}
    bool isEnable() {return m_enable;}
    QString getPluginName();
    void KeywordSearch(QString keyword,DataQueue<ResultInfo> *searchResult);
    void stopSearch();
    QList<SearchPluginIface::Actioninfo> getActioninfo(int type);
    void openAction(int actionkey, QString key, int type);
    QWidget *detailPage(const ResultInfo &ri);

private:
    void initDetailPage();
    QString m_keyWord;
    QString m_currentActionKey;
    QWidget *m_detailPage = nullptr;
    QVBoxLayout *m_detailLyt = nullptr;
    QLabel *m_iconLabel = nullptr;
    QFrame *m_actionFrame = nullptr;
    QVBoxLayout *m_actionFrameLyt = nullptr;
    ActionLabel *m_actionLabel1 = nullptr;

    QVBoxLayout * m_actionLyt = nullptr;

    bool m_enable = true;
    QList<WebSearchPlugin::Actioninfo> m_actionInfo;
};
}
#endif // WEBSEARCHPLUGIN_H
