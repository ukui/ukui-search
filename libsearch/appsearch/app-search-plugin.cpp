#include "app-search-plugin.h"
#include <gio/gdesktopappinfo.h>
#include <QWidget>
#include <QLabel>
#include "file-utils.h"
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
    m_pool.setMaxThreadCount(1);
    m_pool.setExpiryTimeout(1000);
    initDetailPage();
}

const QString AppSearchPlugin::name()
{
    return "Applications Search";
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

QWidget *AppSearchPlugin::detailPage(const ResultInfo &ri)
{
    m_currentActionKey = ri.actionKey;
    m_iconLabel->setPixmap(ri.icon.pixmap(120, 120));
    QFontMetrics fontMetrics = m_nameLabel->fontMetrics();
    QString showname = fontMetrics.elidedText(ri.name, Qt::ElideRight, 215); //当字体长度超过215时显示为省略号
    m_nameLabel->setText(FileUtils::setAllTextBold(showname));
    if(QString::compare(showname, ri.name)) {
        m_nameLabel->setToolTip(ri.name);
    } else {
        m_nameLabel->setToolTip("");
    }
    m_pluginLabel->setText(tr("Application"));
    if(ri.type == 1) {
        m_actionLabel1->hide();
        m_actionLabel2->hide();
        m_actionLabel3->hide();
        m_actionLabel4->show();
        QString showDesc = fontMetrics.elidedText(ri.description.at(0).key + " " + ri.description.at(0).value, Qt::ElideRight, 3114); //当字体长度超过215时显示为省略号
        m_descLabel->setText(FileUtils::escapeHtml(showDesc));
        m_descFrame->show();
        m_line_2->show();

    } else {
        m_descFrame->hide();
        m_line_2->hide();
        m_actionLabel1->show();
        m_actionLabel2->show();
        m_actionLabel3->show();
        m_actionLabel4->hide();
    }
    return m_detailPage;
}

void AppSearchPlugin::initDetailPage()
{
    m_detailPage = new QWidget();
    m_detailPage->setFixedWidth(360);
    m_detailPage->setAttribute(Qt::WA_TranslucentBackground);
    m_detailLyt = new QVBoxLayout(m_detailPage);
    m_detailLyt->setContentsMargins(8, 0, 16, 0);
    m_iconLabel = new QLabel(m_detailPage);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    m_iconLabel->setFixedHeight(128);

    m_nameFrame = new QFrame(m_detailPage);
    m_nameFrameLyt = new QHBoxLayout(m_nameFrame);
    m_nameFrame->setLayout(m_nameFrameLyt);
    m_nameFrameLyt->setContentsMargins(8, 0, 0, 0);
    m_nameLabel = new QLabel(m_nameFrame);
    m_nameLabel->setMaximumWidth(280);
    m_pluginLabel = new QLabel(m_nameFrame);
    m_pluginLabel->setEnabled(false);
    m_nameFrameLyt->addWidget(m_nameLabel);
    m_nameFrameLyt->addStretch();
    m_nameFrameLyt->addWidget(m_pluginLabel);

    m_line_1 = new SeparationLine(m_detailPage);
    m_descFrame = new QFrame(m_detailPage);
    m_descFrameLyt = new QVBoxLayout(m_descFrame);
    m_descLabel = new QLabel(m_descFrame);
    m_descLabel->setTextFormat(Qt::PlainText);
    m_descLabel->setWordWrap(true);
    m_descFrameLyt->addWidget(m_descLabel);
    m_descFrame->setLayout(m_descFrameLyt);
    m_descFrameLyt->setContentsMargins(8, 0, 0, 0);
    m_line_2 = new SeparationLine(m_detailPage);

    m_actionFrame = new QFrame(m_detailPage);
    m_actionFrameLyt = new QVBoxLayout(m_actionFrame);
    m_actionFrameLyt->setContentsMargins(8, 0, 0, 0);
    m_actionLabel1 = new ActionLabel(tr("Open"), m_currentActionKey, m_actionFrame);
    m_actionLabel2 = new ActionLabel(tr("Add Shortcut to Desktop"), m_currentActionKey, m_actionFrame);
    m_actionLabel3 = new ActionLabel(tr("Add Shortcut to Panel"), m_currentActionKey, m_actionFrame);
    m_actionLabel4 = new ActionLabel(tr("Install"), m_currentActionKey, m_actionFrame);

    m_actionFrameLyt->addWidget(m_actionLabel1);
    m_actionFrameLyt->addWidget(m_actionLabel2);
    m_actionFrameLyt->addWidget(m_actionLabel3);
    m_actionFrameLyt->addWidget(m_actionLabel4);
    m_actionFrame->setLayout(m_actionFrameLyt);

    m_detailLyt->addSpacing(50);
    m_detailLyt->addWidget(m_iconLabel);
    m_detailLyt->addWidget(m_nameFrame);
    m_detailLyt->addWidget(m_line_1);
    m_detailLyt->addWidget(m_descFrame);
    m_detailLyt->addWidget(m_line_2);
    m_detailLyt->addWidget(m_actionFrame);
    m_detailPage->setLayout(m_detailLyt);
    m_detailLyt->addStretch();

    connect(m_actionLabel1, &ActionLabel::actionTriggered, [ & ](){
        launch(m_currentActionKey);
    });
    connect(m_actionLabel2, &ActionLabel::actionTriggered, [ & ](){
        addDesktopShortcut(m_currentActionKey);
    });
    connect(m_actionLabel3, &ActionLabel::actionTriggered, [ & ](){
        addPanelShortcut(m_currentActionKey);
    });
    connect(m_actionLabel4, &ActionLabel::actionTriggered, [ & ](){
        installAppAction(m_currentActionKey);
    });
}

//bool AppSearchPlugin::isPreviewEnable(QString key, int type)
//{
//    return false;
//}

//QWidget *AppSearchPlugin::previewPage(QString key, int type, QWidget *parent = nullptr)
//{
//    return nullptr;
//}

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
            qWarning() << "Add shortcut to panel failed, because it is already existed!";
            return false;
        }
        QDBusReply<QVariant> ret = iface.call("AddToTaskbar", path);
        qDebug() << "Add shortcut to panel successed!";
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
    AppMatch::getAppMatch()->startMatchApp(m_keyword, m_uniqueSymbol, m_search_result);
}
