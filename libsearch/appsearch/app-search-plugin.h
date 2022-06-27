#ifndef APPSEARCHPLUGIN_H
#define APPSEARCHPLUGIN_H

#include <QObject>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QAction>
#include <QDBusInterface>
#include <QDBusReply>
#include <QtDBus>
#include "search-plugin-iface.h"
#include "action-label.h"
#include "separation-line.h"
#include "libsearch_global.h"
#include "ukui-search-task.h"
namespace UkuiSearch {
class LIBSEARCH_EXPORT AppSearchPlugin : public QThread, public SearchPluginIface
{
    friend class AppSearch;
    friend class AppMatch;
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
    void stopSearch();
    QList<SearchPluginIface::Actioninfo> getActioninfo(int type);
    void openAction(int actionkey, QString key, int type);
//    bool isPreviewEnable(QString key, int type);
//    QWidget *previewPage(QString key, int type, QWidget *parent);
    QWidget *detailPage(const ResultInfo &ri);
    void run() override;
private:
    void initDetailPage();
    bool launch(const QString &path);
    bool addPanelShortcut(const QString &path);
    bool addDesktopShortcut(const QString &path);
    bool installAppAction(const QString &name);
    bool m_enable = true;
    QList<SearchPluginIface::Actioninfo> m_actionInfo_installed;
    QList<SearchPluginIface::Actioninfo> m_actionInfo_not_installed;
//    QThreadPool m_pool;
    QTimer *m_timer;
    static size_t uniqueSymbol;
    static QMutex m_mutex;

    UkuiSearchTask *m_appSearchTask = nullptr;
    DataQueue<ResultItem>* m_appSearchResults = nullptr;
    DataQueue<SearchPluginIface::ResultInfo> *m_searchResult = nullptr;

    QString m_currentActionKey;
    QWidget *m_detailPage;
    QVBoxLayout *m_detailLyt = nullptr;
    QLabel *m_iconLabel = nullptr;
    QFrame *m_nameFrame = nullptr;
    QHBoxLayout *m_nameFrameLyt = nullptr;
    QLabel *m_nameLabel = nullptr;
    QLabel *m_pluginLabel = nullptr;
    SeparationLine *m_line_1 = nullptr;
    QFrame *m_descFrame = nullptr;
    QLabel *m_descLabel = nullptr;
    QVBoxLayout *m_descFrameLyt = nullptr;
    SeparationLine *m_line_2 = nullptr;
    QFrame *m_actionFrame = nullptr;
    QVBoxLayout *m_actionFrameLyt = nullptr;
    ActionLabel *m_actionLabel1 = nullptr;
    ActionLabel *m_actionLabel2 = nullptr;
    ActionLabel *m_actionLabel3 = nullptr;
    ActionLabel *m_actionLabel4 = nullptr;

    QVBoxLayout * m_actionLyt = nullptr;
Q_SIGNALS:
    void startTimer();
    void stopTimer();
};

//class AppSearch : public QObject, public QRunnable {
//    Q_OBJECT
//public:
//    AppSearch(DataQueue<SearchPluginIface::ResultInfo> *searchResult, DataQueue<ResultItem>* appSearchResults, UkuiSearchTask *appSearchTask, QString keyword, size_t uniqueSymbol);
//    ~AppSearch();
//protected:
//    void run() override;
//private:

//    bool isUniqueSymbolChanged();

//    size_t m_uniqueSymbol;
//    UkuiSearchTask *m_appSearchTask = nullptr;
//    DataQueue<ResultItem>* m_appSearchResults = nullptr;
//    DataQueue<SearchPluginIface::ResultInfo> *m_search_result = nullptr;

//};
}

#endif // APPSEARCHPLUGIN_H
