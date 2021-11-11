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
    xmlElement();
    initDetailPage();
}

const QString SettingsSearchPlugin::name()
{
    return "Settings Search";
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
    //匹配中英混合列表(一级菜单)
    for (auto i = m_mixSearchList.constBegin(); i != m_mixSearchList.constEnd(); ++i) {
        QMap<QString, QString> regmatch = *i;
        QString key = i.key();
        for (auto t = regmatch.begin(); t != regmatch.end(); ++t) {
            QString englishStr = t.value();
            QString chineseStr = t.key();
            QString englishPath = "";
            QString chinesePath = "";
            //转拼音(支持模糊)
            QStringList pinyinlist = FileUtils::findMultiToneWords(chineseStr);
            for (int i = 0; i < pinyinlist.size() / 2; i++) {
                //首字母
                QString shouzimu = pinyinlist.at(2 * i + 1); // 中文转首字母
                if (shouzimu.contains(keyword, Qt::CaseInsensitive)) {
                    resultInfo.name = ql.language() == QLocale::Chinese ? chineseStr : englishStr;
                    if (resultName.contains(resultInfo.name)) {
                        continue;
                    }
                    resultName.append(resultInfo.name);
                    if(ql.language() == QLocale::English) {
                        englishPath = key + "/" + englishStr;
                        resultInfo.icon = FileUtils::getSettingIcon(englishPath, true);
                        resultInfo.actionKey = englishPath;
                    } else if (ql.language() == QLocale::Chinese) {
                        chinesePath = key + "/" + chineseStr;
                        resultInfo.icon = FileUtils::getSettingIcon(chinesePath, true);
                        resultInfo.actionKey = chinesePath;
                    }
                    searchResult->enqueue(resultInfo);
                    break;
                }
                if (keyword.size() < 2)
                    break;
                //拼音
                QString pinyin = pinyinlist.at(2 * i); // 中文转拼音
                if (pinyin.contains(keyword, Qt::CaseInsensitive)) {
                    resultInfo.name = ql.language() == QLocale::Chinese ? chineseStr : englishStr;
                    if (resultName.contains(resultInfo.name)) {
                        continue;
                    }
                    resultName.append(resultInfo.name);
                    if(ql.language() == QLocale::English) {
                        englishPath = key + "/" + englishStr;
                        resultInfo.icon = FileUtils::getSettingIcon(englishPath, true);
                        resultInfo.actionKey = englishPath;
                    } else if (ql.language() == QLocale::Chinese) {
                        chinesePath = key + "/" + chineseStr;
                        resultInfo.icon = FileUtils::getSettingIcon(chinesePath, true);
                        resultInfo.actionKey = chinesePath;
                    }
                    searchResult->enqueue(resultInfo);
                    break;
                }
            }
            //中文名
            if (chineseStr.contains(keyword, Qt::CaseInsensitive)) {
                resultInfo.name = ql.language() == QLocale::Chinese ? chineseStr : englishStr;
                if (resultName.contains(resultInfo.name)) {
                    continue;
                }
                resultName.append(resultInfo.name);
                chinesePath = key + "/" + chineseStr;
                resultInfo.icon = FileUtils::getSettingIcon(chinesePath, true);
                resultInfo.actionKey = chinesePath;
                searchResult->enqueue(resultInfo);
            }
            //英文名
            if (englishStr.contains(keyword, Qt::CaseInsensitive)) {
                resultInfo.name = ql.language() == QLocale::Chinese ? chineseStr : englishStr;
                if (resultName.contains(resultInfo.name)) {
                    continue;
                }
                resultName.append(resultInfo.name);
                englishPath = key + "/" + englishStr;
                resultInfo.icon = FileUtils::getSettingIcon(englishPath, true);
                resultInfo.actionKey = englishPath;
                searchResult->enqueue(resultInfo);
            }
        }
    }
    //匹配中文列表(二级菜单)
    for (auto i = m_chineseSearchList.constBegin(); i != m_chineseSearchList.constEnd(); ++i) {
        QStringList regmatch = *i;
        QString key = i.key();
        for (int t = 0; t < regmatch.size(); t++) {
            QString str = regmatch.at(t);
            QString path = "";
            //直接匹配
            if (str.contains(keyword)) {
                resultInfo.name = ql.language() == QLocale::Chinese ? str : m_englishSearchList[key].at(t);
                if (resultName.contains(resultInfo.name)) {
                    continue;
                }
                resultName.append(resultInfo.name);
                path = key + "/" + str;
                resultInfo.icon = FileUtils::getSettingIcon(path, true);
                resultInfo.actionKey = path;
                searchResult->enqueue(resultInfo);
                continue;
            }
            //转拼音(支持模糊)
            pinyinlist = FileUtils::findMultiToneWords(str);
            for (int i = 0; i < pinyinlist.size() / 2; i++) {
                //首字母匹配
                QString shouzimu = pinyinlist.at(2 * i + 1); // 中文转首字母
                if (shouzimu.contains(keyword, Qt::CaseInsensitive)) {
                    resultInfo.name = ql.language() == QLocale::Chinese ? str : m_englishSearchList[key].at(t);
                    if (resultName.contains(resultInfo.name)) {
                        continue;
                    }
                    resultName.append(resultInfo.name);
                    path = key + "/" + str;
                    resultInfo.icon = FileUtils::getSettingIcon(path, true);
                    resultInfo.actionKey = path;
                    searchResult->enqueue(resultInfo);
                    break;
                }
                if (keyword.size() < 2)
                    break;
                //拼音匹配
                QString pinyin = pinyinlist.at(2 * i); // 中文转拼音
                if (pinyin.contains(keyword, Qt::CaseInsensitive)) {
                    resultInfo.name = ql.language() == QLocale::Chinese ? str : m_englishSearchList[key].at(t);
                    if (resultName.contains(resultInfo.name)) {
                        continue;
                    }
                    resultName.append(resultInfo.name);
                    path = key + "/" + str;
                    resultInfo.icon = FileUtils::getSettingIcon(path, true);
                    resultInfo.actionKey = path;
                    searchResult->enqueue(resultInfo);
                    break;
                }
            }
        }
    }
    //匹配英文列表(二级菜单)
    for (auto i = m_englishSearchList.constBegin(); i != m_englishSearchList.constEnd(); ++i) {
        QStringList regmatch = *i;
        QString key = i.key();
        QString path = "";
        for (int t = 0; t < regmatch.size(); t++) {
            QString str = regmatch.at(t);
            if (str.contains(keyword, Qt::CaseInsensitive)) {
                resultInfo.name = ql.language() == QLocale::Chinese ? m_chineseSearchList[key].at(t) : str;
                if (resultName.contains(resultInfo.name)) {
                    continue;
                }
                resultName.append(resultInfo.name);
                path = key + "/" + str;
                resultInfo.icon = FileUtils::getSettingIcon(path, true);
                resultInfo.actionKey = path;
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
        process.startDetached(QString("ukui-control-center -m %1").arg(key.left(key.indexOf("/")).toLower()));
        break;

    default:
        break;
    }
}

QWidget *SettingsSearchPlugin::detailPage(const ResultInfo &ri)
{
    m_currentActionKey = ri.actionKey;
    m_iconLabel->setPixmap(ri.icon.pixmap(120, 120));
    QFontMetrics fontMetrics = m_nameLabel->fontMetrics();
    QString showname = fontMetrics.elidedText(ri.name, Qt::ElideRight, 215); //当字体长度超过215时显示为省略号
    m_nameLabel->setText(QString("<h3 style=\"font-weight:normal;\">%1</h3>").arg(FileUtils::escapeHtml(showname)));
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

/**
 * @brief SettingsSearchPlugin::xmlElement
 * 将xml文件内容读到内存
 */
void SettingsSearchPlugin::xmlElement() {

    QString environment = QProcessEnvironment::systemEnvironment().value("XDG_SESSION_TYPE");
    QString version;
    QFile file(QString::fromLocal8Bit("/usr/share/ukui-control-center/shell/res/search.xml"));
    if(!file.open(QIODevice::ReadOnly)) {
        return;
    }
    QDomDocument doc;
    doc.setContent(&file);
    QDomElement root = doc.documentElement();
    QDomNode node = root.previousSibling();
    node = root.firstChild();

    QString chineseIndex;
    QString englishIndex;
    QStringList chineseSearchResult;
    QStringList englishSearchResult;
    while (!node.isNull()) {
        QDomElement element = node.toElement();
        QDomNodeList list = element.childNodes();
        //通过xml文件的子节点判断父节点是否有问题
        if (list.count() >= 8 && (list.at(6).nodeName() == QString::fromLocal8Bit("EnglishFunc1"))) {
            //直接获取二级菜单英文名
            QString key = list.at(6).toElement().text();
            chineseSearchResult = m_chineseSearchList.value(key);
            englishSearchResult = m_englishSearchList.value(key);
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
                if (n.nodeName() == QString::fromLocal8Bit("ChineseFunc1")) {
                    chineseIndex = n.toElement().text();
                    if (chineseIndex.isEmpty()) {
                        continue;
                    }
                    if (0 == m_mixSearchList[key].count(chineseIndex)) {
                        m_mixSearchList[key].insert(chineseIndex, key);
                    }
                }
                if (n.nodeName() == QString::fromLocal8Bit("ChineseFunc2")) {
                    chineseIndex = n.toElement().text();
                    if (chineseIndex.isEmpty()) {
                        continue;
                    }
                    chineseSearchResult.append(chineseIndex);
                }

                if (n.nodeName() == QString::fromLocal8Bit("EnglishFunc2")) {
                    englishIndex = /*QString::fromLocal8Bit("/") + */n.toElement().text();
                    if (englishIndex.isEmpty()) {
                        continue;
                    }
                    englishSearchResult.append(englishIndex);
                }
            }

            m_chineseSearchList.insert(key, chineseSearchResult);
            m_englishSearchList.insert(key, englishSearchResult);
            node = node.nextSibling();
        } else {
            qWarning() << "There's something wrong with the xml file's item:" << element.attribute("name");
            node = node.nextSibling();
        }
    }
    file.close();
}

void SettingsSearchPlugin::initDetailPage()
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
