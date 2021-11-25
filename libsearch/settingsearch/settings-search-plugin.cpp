#include <QProcess>
#include <QDomDocument>
#include <QProcessEnvironment>
#include <QWidget>
#include "settings-search-plugin.h"
#include "file-utils.h"
using namespace Zeeker;

SettingsSearchPlugin::SettingsSearchPlugin(QObject *parent) : QObject(parent)
{
    SearchPluginIface::Actioninfo open { 0, tr("Open")};
    m_actionInfo << open;
    m_pool.setMaxThreadCount(1);
    m_pool.setExpiryTimeout(1000);
    xmlElement();
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
    if (keyword == "/")
        return;
    QStringList pinyinlist;
    QStringList resultName;
    ResultInfo resultInfo;
    resultInfo.type = 0;
    QLocale ql;
    /*
    if (ql.language() == QLocale::Chinese) {
        for (auto i = m_chineseSearchList.constBegin(); i != m_chineseSearchList.constEnd(); ++i) {
            QStringList regmatch = *i;
            QString key = i.key();
            for (int t = 0; t < regmatch.size(); t++) {
                if (keyword == "/")
                    continue;
                QString str = regmatch.at(t);
                if (str.contains(keyword)) {
                    resultInfo.name = str;//中文名
                    str = key + "/" + str;
                    resultInfo.icon = FileUtils::getSettingIcon(str, true);
                    resultInfo.actionKey = str;
                    searchResult->enqueue(resultInfo);
                    continue;
                }

                pinyinlist = FileUtils::findMultiToneWords(str);
                for (int i = 0; i < pinyinlist.size() / 2; i++) {
                    str = regmatch.at(t);
                    QString shouzimu = pinyinlist.at(2 * i + 1); // 中文转首字母
                    if (shouzimu.contains(keyword, Qt::CaseInsensitive)) {
                        resultInfo.name = str;
                        str = key + "/" + str;
                        resultInfo.icon = FileUtils::getSettingIcon(str, true);
                        resultInfo.actionKey = str;
                        searchResult->enqueue(resultInfo);
                        break;
                    }
                    if (keyword.size() < 2)
                        break;
                    QString pinyin = pinyinlist.at(2 * i); // 中文转拼音
                    if (pinyin.contains(keyword, Qt::CaseInsensitive)) {
                        resultInfo.name = str;
                        str = key + "/" + str;
                        resultInfo.icon = FileUtils::getSettingIcon(str, true);
                        resultInfo.actionKey = str;
                        searchResult->enqueue(resultInfo);
                        break;
                    }
                }
            }
        }
    }
    if (ql.language() == QLocale::English) {
        for (auto i = m_englishSearchList.constBegin(); i != m_englishSearchList.constEnd(); ++i) {
            QStringList regmatch = *i;
            QString key = i.key();
            for (int t = 0; t < regmatch.size(); t++) {
                if (keyword == "/")
                    continue;
                QString str = regmatch.at(t);
                if (str.contains(keyword, Qt::CaseInsensitive)) {
                    resultInfo.name = str;
                    str = key + "/" + str;
                    resultInfo.icon = FileUtils::getSettingIcon(str, true);
                    resultInfo.actionKey = str;
                    searchResult->enqueue(resultInfo);
                }
            }
        }
    }
    */
    for (auto i = m_chineseSearchList.constBegin(); i != m_chineseSearchList.constEnd(); ++i) {
        QStringList regmatch = *i;
        QString key = i.key();
        for (int t = 0; t < regmatch.size(); t++) {
            QString str = regmatch.at(t);
            if (str.contains(keyword)) {
                resultInfo.name = ql.language() == QLocale::Chinese ? str : m_englishSearchList[key].at(t);
                if (resultName.contains(resultInfo.name)) {
                    continue;
                }
                resultName.append(resultInfo.name);
                str = key + "/" + str;
                resultInfo.icon = FileUtils::getSettingIcon();
                resultInfo.actionKey = str;
                searchResult->enqueue(resultInfo);
                continue;
            }
            if (ql.language() == QLocale::Chinese) {
                pinyinlist = FileUtils::findMultiToneWords(str);
                for (int i = 0; i < pinyinlist.size() / 2; i++) {
                    str = regmatch.at(t);
                    QString shouzimu = pinyinlist.at(2 * i + 1); // 中文转首字母
                    if (shouzimu.contains(keyword, Qt::CaseInsensitive)) {
                        resultInfo.name = str;
                        if (resultName.contains(resultInfo.name)) {
                            continue;
                        }
                        resultName.append(resultInfo.name);
                        str = key + "/" + str;
                        resultInfo.icon = FileUtils::getSettingIcon();
                        resultInfo.actionKey = str;
                        searchResult->enqueue(resultInfo);
                        break;
                    }
                    if (keyword.size() < 2)
                        break;
                    QString pinyin = pinyinlist.at(2 * i); // 中文转拼音
                    if (pinyin.contains(keyword, Qt::CaseInsensitive)) {
                        resultInfo.name = str;
                        if (resultName.contains(resultInfo.name)) {
                            continue;
                        }
                        resultName.append(resultInfo.name);
                        str = key + "/" + str;
                        resultInfo.icon = FileUtils::getSettingIcon();
                        resultInfo.actionKey = str;
                        searchResult->enqueue(resultInfo);
                        break;
                    }
                }
            }
        }
    }
    for (auto i = m_englishSearchList.constBegin(); i != m_englishSearchList.constEnd(); ++i) {
        QStringList regmatch = *i;
        QString key = i.key();
        for (int t = 0; t < regmatch.size(); t++) {
            QString str = regmatch.at(t);
            if (str.contains(keyword, Qt::CaseInsensitive)) {
                resultInfo.name = ql.language() == QLocale::Chinese ? m_chineseSearchList[key].at(t) : str;
                if (resultName.contains(resultInfo.name)) {
                    continue;
                }
                resultName.append(resultInfo.name);
                str = key + "/" + str;
                resultInfo.icon = FileUtils::getSettingIcon();
                resultInfo.actionKey = str;
                searchResult->enqueue(resultInfo);
            }
        }
    }
    for (auto i = m_mixSearchList.constBegin(); i != m_mixSearchList.constEnd(); ++i) {
        QMap<QString, QString> regmatch = *i;
        QString key = i.key();
        for (auto t = regmatch.begin(); t != regmatch.end(); ++t) {
            QString englishStr = t.value();
            QString chineseStr = t.key();
            if (chineseStr.contains(keyword, Qt::CaseInsensitive)) {
                resultInfo.name = ql.language() == QLocale::Chinese ? chineseStr : englishStr;
                if (resultName.contains(resultInfo.name)) {
                    continue;
                }
                resultName.append(resultInfo.name);
                chineseStr = key + "/" + chineseStr;
                resultInfo.icon = FileUtils::getSettingIcon();
                resultInfo.actionKey = chineseStr;
                searchResult->enqueue(resultInfo);
            }
            if (englishStr.contains(keyword, Qt::CaseInsensitive)) {
                resultInfo.name = ql.language() == QLocale::Chinese ? chineseStr : englishStr;
                if (resultName.contains(resultInfo.name)) {
                    continue;
                }
                resultName.append(resultInfo.name);
                englishStr = key + "/" + englishStr;
                resultInfo.icon = FileUtils::getSettingIcon();
                resultInfo.actionKey = englishStr;
                searchResult->enqueue(resultInfo);
            }
        }
    }
}

QList<SearchPluginIface::Actioninfo> SettingsSearchPlugin::getActioninfo(int type)
{
    return m_actionInfo;
}

void SettingsSearchPlugin::openAction(int actionkey, QString key, int type)
{
    //TODO add some return message here.
    QProcess  process;
    switch (actionkey) {
    case 0:
        //打开控制面板对应页面
        process.startDetached(QString("ukui-control-center --%1").arg(key.left(key.indexOf("/")).toLower()));
        break;

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
    m_nameLabel->setText(QString("<h3 style=\"font-weight:normal;\">%1</h3>").arg(FileUtils::escapeHtml(showname)));
    if (QString::compare(showname, ri.name)) {
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

/**
 * @brief SettingsSearchPlugin::xmlElement
 * 将xml文件内容读到内存
 */
void SettingsSearchPlugin::xmlElement() {

    QString environment = QProcessEnvironment::systemEnvironment().value("XDG_SESSION_TYPE");
    QString version;
    QFile file(QString::fromLocal8Bit("/usr/share/ukui-control-center/shell/res/search.xml"));
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }
    QDomDocument doc;
    doc.setContent(&file);
    QDomElement root = doc.documentElement();
    QDomNode node = root.previousSibling();
    node = root.firstChild();

    QString chineseIndex;
    QString englishIndex;
    QStringList chineSearchResult;
    QStringList englishSearchResult;
    while (!node.isNull()) {
        QDomElement element = node.toElement();
        QString key = element.attribute("name");
        chineSearchResult = m_chineseSearchList.value(key);
        englishSearchResult = m_englishSearchList.value(key);
        QDomNodeList list = element.childNodes();
        for (int i = 0; i < list.count(); ++i) {
            QDomNode n = list.at(i);

            if (n.nodeName() == QString::fromLocal8Bit("Environment")) {
                version=n.toElement().text();
                if ((version == "v101" && environment == "wayland")
                        || (version == "hw990" && environment == "x11")) {
                    break;
                }
                continue;
            }
            if (n.nodeName() == QString::fromLocal8Bit("ChinesePlugin")) {
                chineseIndex = n.toElement().text();
                if (chineseIndex.isEmpty()) {
                    continue;
                }
                if (0 == m_mixSearchList[key].count(chineseIndex)) {
                    m_mixSearchList[key].insert(chineseIndex, key);
                }
            }
            if (n.nodeName() == QString::fromLocal8Bit("ChineseFunc")) {
                chineseIndex = n.toElement().text();
                if (chineseIndex.isEmpty()) {
                    continue;
                }
                if (chineSearchResult.contains(chineseIndex)) {
                    continue;
                } else {
                    chineSearchResult.append(chineseIndex);
                }
            }

            if (n.nodeName() == QString::fromLocal8Bit("EnglishFunc")) {
                englishIndex = /*QString::fromLocal8Bit("/") + */n.toElement().text();
                if (englishIndex.isEmpty()) {
                    continue;
                }
                englishSearchResult.append(englishIndex);
            }
        }

        m_chineseSearchList.insert(key, chineSearchResult);
        m_englishSearchList.insert(key, englishSearchResult);
        node = node.nextSibling();
    }
    file.close();
}

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

    m_line_1 = new QFrame(m_detailPage);
    m_line_1->setLineWidth(0);
    m_line_1->setFixedHeight(1);
    m_line_1->setStyleSheet("QFrame{background: rgba(0,0,0,0.2);}");

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
    });
}
