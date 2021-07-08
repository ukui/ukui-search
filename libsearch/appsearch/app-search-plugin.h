#ifndef APPSEARCHPLUGIN_H
#define APPSEARCHPLUGIN_H

#include <QObject>
#include "search-plugin-iface.h"
#include "app-match.h"
#include "libsearch_global.h"
namespace Zeeker {
class LIBSEARCH_EXPORT AppSearchPlugin : public QObject, public SearchPluginIface
{
    friend class AppSearch;
    Q_OBJECT
public:
    AppSearchPlugin(QObject *parent = nullptr);
    PluginType pluginType() {return PluginType::SearchPlugin;}
    const QString name();
    const QString description();
    const QIcon icon() {return QIcon::fromTheme("appsearch");}
    void setEnable(bool enable) {m_enable = enable;}
    bool isEnable() {return m_enable;}
    QString getPluginName();

    void KeywordSearch(QString keyword,DataQueue<ResultInfo> *searchResult);
    QList<SearchPluginIface::Actioninfo> getActioninfo(int type);
    void openAction(int actionkey, QString key, int type);
    bool isPreviewEnable(QString key, int type);
    QWidget *previewPage(QString key, int type, QWidget *parent);
private:
    bool launch(const QString &path);
    bool addPanelShortcut(const QString &path);
    bool addDesktopShortcut(const QString &path);
    bool installAppAction(const QString &name);
    bool m_enable = true;
    QList<SearchPluginIface::Actioninfo> m_actionInfo_installed;
    QList<SearchPluginIface::Actioninfo> m_actionInfo_not_installed;
    QThreadPool m_pool;
    static size_t uniqueSymbol;
    static QMutex m_mutex;
};

class AppSearch : public QObject, public QRunnable {
    Q_OBJECT
public:
    AppSearch(DataQueue<SearchPluginIface::ResultInfo> *searchResult, const QString& keyword, size_t uniqueSymbol);
    ~AppSearch();
protected:
    void run() override;
private:
    DataQueue<SearchPluginIface::ResultInfo> *m_search_result = nullptr;
    size_t m_uniqueSymbol;
    QString m_keyword;
    QMap<NameString, QStringList> m_installed_apps;
    QMap<NameString, QStringList> m_not_installed_apps;
};
}

#endif // APPSEARCHPLUGIN_H
