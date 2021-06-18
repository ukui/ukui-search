#include <QProcess>
#include <QDomDocument>
#include <QProcessEnvironment>
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
    QStringList pinyinlist;
    ResultInfo resultInfo;
    resultInfo.type = 0;
    QLocale ql;
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
                    searchResult->append(resultInfo);
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
                        searchResult->append(resultInfo);
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
                        searchResult->append(resultInfo);
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
                    searchResult->append(resultInfo);
                }
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
        if (key.left(key.indexOf("/")).toLower() == "wallpaper")
            process.startDetached(QString("ukui-control-center --background"));
        else
            process.startDetached(QString("ukui-control-center --%1").arg(key.left(key.indexOf("/")).toLower()));
        break;

    default:
        break;
    }
}

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

            if (n.nodeName() == QString::fromLocal8Bit("ChinesePlugin")
                    or n.nodeName() == QString::fromLocal8Bit("ChineseFunc")) {
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
                englishIndex = QString::fromLocal8Bit("/") + n.toElement().text();
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
