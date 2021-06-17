#include "app-search-plugin.h"
#include <gio/gdesktopappinfo.h>
using namespace Zeeker;
size_t AppSearchPlugin::uniqueSymbol = 0;
QMutex  AppSearchPlugin::m_mutex;
AppSearchPlugin::AppSearchPlugin(QObject *parent) : QObject(parent)
{
    SearchPluginIface::Actioninfo open { 0, tr("Open")};
    SearchPluginIface::Actioninfo addtoDesktop { 1, tr("Add Shortcut to Desktop")};
    SearchPluginIface::Actioninfo addtoPanel { 2, tr("Add Shortcut to Panel")};
    SearchPluginIface::Actioninfo install { 0, tr("Install")};
    m_actionInfo_installed << open << addtoDesktop << addtoPanel;
    m_actionInfo_not_installed << install;
    AppMatch::getAppMatch()->start();
    m_pool.setMaxThreadCount(2);
    m_pool.setExpiryTimeout(1000);
}

const QString AppSearchPlugin::name()
{
    return tr("Applications Search");
}

const QString AppSearchPlugin::description()
{
    return tr("Applications Search");
}

QString AppSearchPlugin::getPluginName()
{
    return tr("Applications Search");
}

void AppSearchPlugin::KeywordSearch(QString keyword, DataQueue<SearchPluginIface::ResultInfo> *searchResult)
{
    m_mutex.lock();
    ++uniqueSymbol;
    m_mutex.unlock();
    AppSearch *appsearch = new AppSearch(searchResult, keyword, uniqueSymbol);
    m_pool.start(appsearch);
}

QList<SearchPluginIface::Actioninfo> AppSearchPlugin::getActioninfo(int type)
{
    switch (type) {
    case 0:
        return m_actionInfo_installed;
        break;
    case 1:
        return m_actionInfo_not_installed;
        break;
    default:
        return QList<SearchPluginIface::Actioninfo>();
        break;
    }
}

void AppSearchPlugin::openAction(int actionkey, QString key, int type)
{
    switch (type) {
    case 0:
        switch (actionkey) {
        case 0:
            if(!launch(key)) {
                qWarning() << "Fail to launch:" << key;
            }
            break;
        case 1:
            if(!addDesktopShortcut(key)) {
                qWarning() << "Fail to add Desktop Shortcut:" << key;
            }
            break;
        case 2:
            if(!addPanelShortcut(key)) {
                qWarning() << "Fail to add Panel Shortcut:" << key;
            }
            break;
        default:
            break;
        }
        break;
    case 1:
        if(!installAppAction(key)) {
            qWarning() << "Fail to install:" << key;
        }
        break;
    default:
        break;
    }
}

bool AppSearchPlugin::launch(const QString &path)
{
    GDesktopAppInfo * desktopAppInfo = g_desktop_app_info_new_from_filename(path.toLocal8Bit().data());
    bool res = static_cast<bool>(g_app_info_launch(G_APP_INFO(desktopAppInfo), nullptr, nullptr, nullptr));
    g_object_unref(desktopAppInfo);
    return res;
}
bool AppSearchPlugin::addPanelShortcut(const QString& path) {
    QDBusInterface iface("com.ukui.panel.desktop",
                         "/",
                         "com.ukui.panel.desktop",
                         QDBusConnection::sessionBus());
    if(iface.isValid()) {
        QDBusReply<bool> isExist = iface.call("CheckIfExist", path);
        if(isExist) {
            qWarning() << "qDebug: Add shortcut to panel failed, because it is already existed!";
            return false;
        }
        QDBusReply<QVariant> ret = iface.call("AddToTaskbar", path);
        qDebug() << "qDebug: Add shortcut to panel successed!";
        return true;
    }
    return false;
}

bool AppSearchPlugin::addDesktopShortcut(const QString& path) {
    QString dirpath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QFileInfo fileInfo(path);
    QString desktopfn = fileInfo.fileName();
    QFile file(path);
    QString newName = QString(dirpath + "/" + desktopfn);
    bool ret = file.copy(QString(dirpath + "/" + desktopfn));
    if(ret) {
        QProcess process;
        process.startDetached(QString("chmod a+x %1").arg(newName));
        return true;
    }
    return false;
}

bool AppSearchPlugin::installAppAction(const QString & name) {
    QDBusInterface * interface = new QDBusInterface("com.kylin.softwarecenter",
                "/com/kylin/softwarecenter",
                "com.kylin.utiliface",
                QDBusConnection::sessionBus());

    if(interface->isValid()) {
        //软件商店已打开，直接跳转
        interface->call("show_search_result", name);
        bool reply = QDBusReply<bool>(interface->call(QString("show_search_result"), name));
        return reply;
    } else {
        //软件商店未打开，打开软件商店下载此软件
        qDebug() << "Softwarecenter has not been launched, now launch it." << name;
        QProcess process;
        return process.startDetached(QString("kylin-software-center -find %1").arg(name));
    }
}

AppSearch::AppSearch(DataQueue<SearchPluginIface::ResultInfo> *searchResult, const QString &keyword, size_t uniqueSymbol)
{
    this->setAutoDelete(true);
    m_search_result = searchResult;
    m_keyword = keyword;
    m_uniqueSymbol = uniqueSymbol;
}

AppSearch::~AppSearch()
{
}

void AppSearch::run()
{
    //These weird code is mean to be compatible with the old version UI.
    AppMatch::getAppMatch()->startMatchApp(m_keyword, m_installed_apps, m_not_installed_apps);
    QMapIterator<NameString, QStringList> i(m_installed_apps);
    while (i.hasNext()) {
        i.next();
        SearchPluginIface::ResultInfo ri;
        if(!QIcon::fromTheme(i.value().at(1)).isNull()) {
            ri.icon = QIcon::fromTheme(i.value().at(1));
        }else {
            ri.icon = QIcon(":/res/icons/desktop.png");
        }
        ri.name = i.key().app_name;
        ri.actionKey = i.value().at(0);
        ri.type = 0; //0 means installed apps.
        if (m_uniqueSymbol == AppSearchPlugin::uniqueSymbol) {
            m_search_result->enqueue(ri);
        } else {
            break;
        }
    }
    QMapIterator<NameString, QStringList> in(m_not_installed_apps);
    while (in.hasNext()) {
        in.next();
        SearchPluginIface::ResultInfo ri;
        if(!QIcon(in.value().at(1)).isNull()) {
            ri.icon = QIcon(in.value().at(1));
        }else {
            ri.icon = QIcon(":/res/icons/desktop.png");
        }
        ri.name = in.key().app_name;
        SearchPluginIface::DescriptionInfo di;
        di.key = QString(tr("Application Description:"));
        di.value = in.value().at(3);
        ri.description.append(di);
        ri.actionKey = in.value().at(2);
        ri.type = 1; //1 means not installed apps.
        if (m_uniqueSymbol == AppSearchPlugin::uniqueSymbol) {
            m_search_result->enqueue(ri);
        } else {
            break;
        }
    }
}
