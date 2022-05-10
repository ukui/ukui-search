#include <QProcess>
#include <QDomDocument>
#include <QProcessEnvironment>
#include <QWidget>
#include <QDBusReply>
#include "settings-search-plugin.h"
#include "file-utils.h"

using namespace Zeeker;
static SettingsMatch * settingMatchClass = nullptr;
size_t SettingsSearchPlugin::m_uniqueSymbolForSettings = 0;
QMutex SettingsSearchPlugin::m_mutex;

SettingsSearchPlugin::SettingsSearchPlugin(QObject *parent) : QObject(parent)
{
    SearchPluginIface::Actioninfo open { 0, tr("Open")};
    m_actionInfo << open;
    SettingsMatch::getInstance()->start();
    m_pool.setMaxThreadCount(1);
    m_pool.setExpiryTimeout(1000);
    initDetailPage();
}

const QString SettingsSearchPlugin::name()
{
    return tr("Settings Search");
}

const QString SettingsSearchPlugin::description()
{
    return tr("Settings search.");
}

QString SettingsSearchPlugin::getPluginName()
{
    return tr("Settings Search");
}

void Zeeker::SettingsSearchPlugin::KeywordSearch(QString keyword, DataQueue<ResultInfo> *searchResult)
{
    m_mutex.lock();
    ++m_uniqueSymbolForSettings;
    m_mutex.unlock();
    SettingsSearch *settingSearch = new SettingsSearch(searchResult, keyword, m_uniqueSymbolForSettings);
    m_pool.start(settingSearch);
}

QList<SearchPluginIface::Actioninfo> SettingsSearchPlugin::getActioninfo(int type)
{
    return m_actionInfo;
}

void SettingsSearchPlugin::openAction(int actionkey, QString key, int type)
{
    //TODO add some return message here.
    switch (actionkey) {
    case 0:
    {
        bool res(false);
        QDBusInterface * appLaunchInterface = new QDBusInterface("com.kylin.AppManager",
                                                                 "/com/kylin/AppManager",
                                                                 "com.kylin.AppManager",
                                                                 QDBusConnection::sessionBus());
        if(!appLaunchInterface->isValid()) {
            qWarning() << qPrintable(QDBusConnection::sessionBus().lastError().message());
            res = false;
        } else {
            appLaunchInterface->setTimeout(10000);
            QDBusReply<bool> reply = appLaunchInterface->call("LaunchAppWithArguments", "ukui-control-center.desktop", QStringList() << "-m" << key.toLower());
            if(reply.isValid()) {
                res = reply;
            } else {
                qWarning() << "SoftWareCenter dbus called failed!";
                res = false;
            }
        }
        if(appLaunchInterface) {
            delete appLaunchInterface;
        }
        appLaunchInterface = NULL;
        if (res)
            break;
        //打开控制面板对应页面
        QProcess  process;
        process.startDetached(QString("ukui-control-center --%1").arg(key.left(key.indexOf("/")).toLower()));
        break;
    }
    default:
        break;
    }
}

QWidget *SettingsSearchPlugin::detailPage(const ResultInfo &ri)
{
    m_currentActionKey = ri.actionKey;
    m_iconLabel->setPixmap(ri.icon.pixmap(240, 240));
    QFontMetrics fontMetrics = m_nameLabel->fontMetrics();
    QString showname = fontMetrics.elidedText(ri.name, Qt::ElideRight, 215); //当字体长度超过215时显示为省略号
    m_nameLabel->setText(FileUtils::setAllTextBold(showname));
    if(QString::compare(showname, ri.name)) {
        m_nameLabel->setToolTip(ri.name);
    }
    return m_detailPage;
}

//bool SettingsSearchPlugin::isPreviewEnable(QString key, int type)
//{
//    return false;
//}

//QWidget *SettingsSearchPlugin::previewPage(QString key, int type, QWidget *parent = nullptr)
//{
//    return nullptr;
//}

void SettingsSearchPlugin::initDetailPage()
{
    m_detailPage = new QWidget();
    m_detailPage->setFixedWidth(421);
    m_detailPage->setAttribute(Qt::WA_TranslucentBackground);
    m_detailLyt = new QVBoxLayout(m_detailPage);
    m_detailLyt->setContentsMargins(8, 0, 16, 0);
    m_iconLabel = new QLabel(m_detailPage);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    m_iconLabel->setFixedHeight(240);

    m_nameFrame = new QFrame(m_detailPage);
    m_nameFrameLyt = new QHBoxLayout(m_nameFrame);
    m_nameFrame->setLayout(m_nameFrameLyt);
    m_nameFrameLyt->setContentsMargins(8, 0, 0, 0);
    m_nameLabel = new QLabel(m_nameFrame);
    m_nameLabel->setMaximumWidth(280);
    m_pluginLabel = new QLabel(m_nameFrame);
    m_pluginLabel->setText(tr("Settings"));
    m_pluginLabel->setEnabled(false);
    m_nameFrameLyt->addWidget(m_nameLabel);
    m_nameFrameLyt->addStretch();
    m_nameFrameLyt->addWidget(m_pluginLabel);

    m_line_1 = new SeparationLine(m_detailPage);

    m_actionFrame = new QFrame(m_detailPage);
    m_actionFrameLyt = new QVBoxLayout(m_actionFrame);
    m_actionFrameLyt->setContentsMargins(8, 0, 0, 0);
    m_actionLabel1 = new ActionLabel(tr("Open"), m_currentActionKey, m_actionFrame);

    m_actionFrameLyt->addWidget(m_actionLabel1);;
    m_actionFrame->setLayout(m_actionFrameLyt);

    m_detailLyt->addSpacing(50);
    m_detailLyt->addWidget(m_iconLabel);
    m_detailLyt->addWidget(m_nameFrame);
    m_detailLyt->addWidget(m_line_1);
    m_detailLyt->addWidget(m_actionFrame);
    m_detailPage->setLayout(m_detailLyt);
    m_detailLyt->addStretch();

    connect(m_actionLabel1, &ActionLabel::actionTriggered, [ & ](){
        openAction(0, m_currentActionKey, 0);
        QDBusMessage res = QDBusConnection::sessionBus().call(QDBusMessage::createMethodCall("com.ukui.search.service",
                                                                                             "/",
                                                                                             "org.ukui.search.service",
                                                                                             "hideWindow"));
    });
}

SettingsSearch::SettingsSearch(DataQueue<SearchPluginIface::ResultInfo> *searchResult, const QString &keyword, size_t uniqueSymbol)
{
    this->setAutoDelete(true);
    this->m_searchResult = searchResult;
    this->m_keyword = keyword;
    this->m_uniqueSymbol = uniqueSymbol;
}

void SettingsSearch::run()
{
    SettingsMatch::getInstance()->startSearch(m_keyword, m_uniqueSymbol, m_searchResult);
}

SettingsMatch *SettingsMatch::getInstance()
{
    if (!settingMatchClass) {
        settingMatchClass = new SettingsMatch;
    }
    return settingMatchClass;
}

void SettingsMatch::run()
{
    this->initDataOfXml();
}

/**
 * @brief SettingsMatch::initDataOfXml
 * 将xml文件内容读到内存
 */

void SettingsMatch::initDataOfXml(){
    QFile file(QString::fromLocal8Bit("/usr/share/ukui-control-center/shell/res/search.xml"));//xml文件所在路径
    if(!file.open(QIODevice::ReadOnly)) {
        return;
    }
    QDomDocument doc;
    doc.setContent(&file);
    file.close();

    QDomElement root = doc.documentElement();
    QDomNode node = root.previousSibling();
    node = root.firstChild();
    matchNodes(node);
    qDebug() << "Init Settings Data Finished";
}

bool SettingsMatch::matchCommonEnvironment(QDomNode childNode)
{
    if (childNode.nodeName() == QString::fromLocal8Bit("Environment")) {
        QString environment = QProcessEnvironment::systemEnvironment().value("XDG_SESSION_TYPE");
        QString version = childNode.toElement().text();
        if ((version == "v101" && environment == "wayland")
                || (version == "hw990" && environment == "x11")) {
            return false;
        }
    }
    return true;
}

void SettingsMatch::matchNodes(QDomNode node){
    QString chineseIndex;
    QString englishIndex;
    QStringList englishSearchResult;

    while (!node.isNull()) {
        QDomElement element = node.toElement();
        QDomNodeList list = element.childNodes();

        QString enNameOfPlugin = element.attribute("name");
        QString enNameOfFunc = list.at(0).nextSiblingElement("EnglishFunc").toElement().text();

        englishSearchResult = m_searchMap.value(enNameOfPlugin);

        for (int i = 0; i < list.count(); ++i) {
            QDomNode childNode = list.at(i);
            QStringList aItemInfo;

            chineseIndex = childNode.toElement().text();

            if (!matchCommonEnvironment(childNode)) {
                break;
            }
            //TODO: Refactoring this part of code.
            if (childNode.nodeName() == QString::fromLocal8Bit("ChinesePlugin")) {
                englishIndex = enNameOfPlugin;
                aItemInfo << englishIndex << chineseIndex << FileUtils::findMultiToneWords(chineseIndex);
                m_dataMap.insert(enNameOfPlugin, aItemInfo);
            }

            if (childNode.nodeName() == QString::fromLocal8Bit("ChineseFunc")) {
                englishIndex = enNameOfFunc;
                englishSearchResult.append(englishIndex);
                aItemInfo << englishIndex << chineseIndex << FileUtils::findMultiToneWords(chineseIndex);
                m_dataMap.insert(enNameOfPlugin + "/" + enNameOfFunc, aItemInfo);
            }
        }

        m_searchMap.insert(enNameOfPlugin, englishSearchResult);
        node = node.nextSibling();
    }
}

void SettingsMatch::startSearch(QString &keyword, size_t uniqueSymbol, DataQueue<SearchPluginIface::ResultInfo> *searchResult)
{
    if (keyword == "/") {
        return;
    }
    //根据searchMap的对应关系匹配dataMap中的数据信息
    for (auto i = m_searchMap.constBegin(); i != m_searchMap.constEnd(); ++i) {
        QStringList regMatch = *i;
        QString pluginName = i.key();
        QString keyOfSecondLv = QString("%1").arg(pluginName);
        matchDataMap(keyOfSecondLv, keyword, uniqueSymbol, searchResult);
        for (QString funcName: regMatch) {
            QString keyOfThridLv = keyOfSecondLv + QString("/%1").arg(funcName);
            matchDataMap(keyOfThridLv, keyword, uniqueSymbol, searchResult);
        }
    }
}

void SettingsMatch::matchDataMap(QString &key, QString &keyword, size_t uniqueSymbol, DataQueue<SearchPluginIface::ResultInfo> *searchResult)
{
    SearchPluginIface::ResultInfo resultInfo;
    resultInfo.type = 0;
    for (int counter = 0; counter < m_dataMap.value(key).size(); ++counter) {
        QString data = m_dataMap.value(key).at(counter);
        //关键字为单字不搜拼音
        if (keyword.size() < 2 and counter > 1 and counter % 2 == 0) {
            continue;
        }
        if (data.contains(keyword, Qt::CaseInsensitive)) {
            createResultInfo(resultInfo, m_dataMap.value(key), key);
            //判断是否为同一次搜索
            SettingsSearchPlugin::m_mutex.lock();
            if (uniqueSymbol == SettingsSearchPlugin::m_uniqueSymbolForSettings) {
                searchResult->enqueue(resultInfo);
                SettingsSearchPlugin::m_mutex.unlock();
                break;
            } else {
                SettingsSearchPlugin::m_mutex.unlock();
                return;
            }
        }
    }
}

void SettingsMatch::createResultInfo(SearchPluginIface::ResultInfo &resultInfo, const QStringList &itemInfo, const QString &path)
{
    QLocale ql;
    resultInfo.name = ql.language() == QLocale::English ? itemInfo.at(0) : itemInfo.at(1);
    resultInfo.icon = FileUtils::getSettingIcon();
    resultInfo.actionKey = path.section("/", 0, 0);
}
