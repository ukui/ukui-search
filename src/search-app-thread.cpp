#include "search-app-thread.h"

using namespace Zeeker;
size_t uniqueSymbol = 0;
QMutex  m_mutex;

SearchAppThread::SearchAppThread(QObject *parent) : QObject(parent) {
    m_pool.setMaxThreadCount(2);
    m_pool.setExpiryTimeout(1000);
}

void SearchAppThread::startSearch(const QString & keyword) {
    m_mutex.lock();
    ++uniqueSymbol;
    m_mutex.unlock();
    SearchApp *appsearch;
    appsearch = new SearchApp(keyword,uniqueSymbol,this);
//    appsearch->setKeyword(keyword);
//    connect(appsearch, &SearchApp::searchResultApp, this, &SearchAppThread::searchResultApp);
    m_pool.start(appsearch);
}

void SearchAppThread::sendResult(const QVector<QStringList> result)
{
    Q_EMIT this->searchResultApp(result);
}


SearchApp::SearchApp(const QString& keyword, size_t uniqueSymbol, QObject * parent) : QObject(parent) {
    this->setAutoDelete(true);
    m_searchappThread = qobject_cast<SearchAppThread *>(parent);
    m_keyword = keyword;
    m_uniqueSymbol = uniqueSymbol;
}

SearchApp::~SearchApp() {
}

///**
// * @brief SearchAppThread::startSearch 激活线程
// * @param keyword 关键词
// */
//void SearchApp::setKeyword(const QString & keyword)
//{
//    m_keyword = keyword;
//}

void SearchApp::run() {
    //nameList:应用名，pathList:已安装的是.desktop路径，未安装为空，iconList:已安装的是图标名，未安装的是图标路径
    QStringList nameList, pathList, iconList, descList;
    QVector<QStringList> appVector;
    AppMatch::getAppMatch()->startMatchApp(m_keyword, m_installed_apps, m_uninstalled_apps);
    QMapIterator<NameString, QStringList> installed_iter(m_installed_apps);
    while(installed_iter.hasNext()) {
        installed_iter.next();
        nameList << installed_iter.key().app_name;
        pathList << installed_iter.value().at(0);
        iconList << installed_iter.value().at(1);
        descList << installed_iter.value().at(3);
    }
    QMapIterator<NameString, QStringList> uninstalled_iter(m_uninstalled_apps);
    while(uninstalled_iter.hasNext()) {
        uninstalled_iter.next();
        QString name;
        //当返回列表的value中含包名时，将名称按“应用名/包名”的格式存储
        if(!uninstalled_iter.value().at(2).isEmpty() && uninstalled_iter.value().at(2) != "") {
            name = uninstalled_iter.key().app_name + "/" + uninstalled_iter.value().at(2);
        } else name = uninstalled_iter.key().app_name;
        nameList << name;
        pathList << uninstalled_iter.value().at(0);
        iconList << uninstalled_iter.value().at(1);
        descList << uninstalled_iter.value().at(3);
    }
    appVector.append(nameList);
    appVector.append(pathList);
    appVector.append(iconList);
    appVector.append(descList);
    m_mutex.lock();
    if (m_uniqueSymbol == uniqueSymbol) {
        QMetaObject::invokeMethod(m_searchappThread, "sendResult", Q_ARG(const QVector<QStringList>, appVector));
//        Q_EMIT this->searchResultApp(appVector);
    }
    m_mutex.unlock();
    m_installed_apps.clear();
    m_uninstalled_apps.clear();
}


