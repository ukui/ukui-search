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
#include <QDomDocument>
#include "action-label.h"
#include "separation-line.h"
#include "search-plugin-iface.h"

namespace UkuiSearch {
class LIBSEARCH_EXPORT SettingsSearchPlugin : public QObject, public SearchPluginIface
{
    Q_OBJECT
    friend class SettingsSearch;
    friend class SettingsMatch;
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
    void initDetailPage();
    bool m_enable = true;
    QList<SettingsSearchPlugin::Actioninfo> m_actionInfo;
    QThreadPool m_pool;
    static size_t m_uniqueSymbolForSettings;
    static QMutex m_mutex;

    QString m_currentActionKey;
    QWidget *m_detailPage = nullptr;
    QVBoxLayout *m_detailLyt = nullptr;
    QLabel *m_iconLabel = nullptr;
    QFrame *m_nameFrame = nullptr;
    QHBoxLayout *m_nameFrameLyt = nullptr;
    QLabel *m_nameLabel = nullptr;
    QLabel *m_pluginLabel = nullptr;
    SeparationLine *m_line_1 = nullptr;
    QFrame *m_actionFrame = nullptr;
    QVBoxLayout *m_actionFrameLyt = nullptr;
    ActionLabel *m_actionLabel1 = nullptr;
    QVBoxLayout * m_actionLyt = nullptr;
};

class SettingsSearch :public QObject, public QRunnable {
    Q_OBJECT
public:
    SettingsSearch(DataQueue<SearchPluginIface::ResultInfo> *searchResult, const QString& keyword, size_t uniqueSymbol);
    ~SettingsSearch() = default;
protected:
    void run() override;
private:
    QString m_keyword;
    size_t m_uniqueSymbol;
    DataQueue<SearchPluginIface::ResultInfo> *m_searchResult = nullptr;
};

class SettingsMatch :public QObject , public QThread {
public:
    static SettingsMatch *getInstance();
    void startSearch(QString &keyword, size_t uniqueSymbol, DataQueue<SearchPluginIface::ResultInfo> *searchResult);
protected:
    void run() override;
private:
    explicit SettingsMatch() = default;
    ~SettingsMatch() = default;
    void initDataOfXml();
    bool matchCommonEnvironment(QDomNode childNode);
    void matchNodes(QDomNode node);
    void matchDataMap(QString &key, QString &keyword, size_t uniqueSymbol, DataQueue<SearchPluginIface::ResultInfo> *searchResult);
    void createResultInfo(SearchPluginIface::ResultInfo &resultInfo, const QStringList &itemInfo, const QString &path);
    QMap<QString, QMap<QString, QStringList>> m_searchMap;
    QMap<QString, QStringList> m_dataMap;
};
}
#endif // SETTINGSSEARCHPLUGIN_H
