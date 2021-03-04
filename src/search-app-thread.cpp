#include "search-app-thread.h"

SearchAppThread::SearchAppThread(QObject * parent) : QThread(parent)
{
    m_appMatch = new AppMatch(this);
}

SearchAppThread::~SearchAppThread()
{
    if (m_appMatch) {
        delete m_appMatch;
        m_appMatch = NULL;
    }
}

/**
 * @brief SearchAppThread::startSearch 激活线程
 * @param keyword 关键词
 */
void SearchAppThread::startSearch(const QString & keyword)
{
    m_keyword = keyword;
    m_stop = false;
    this->start();
}

void SearchAppThread::stop()
{
    m_stop = true;
    this->quit();
    this->wait();
}

void SearchAppThread::run()
{
    //nameList:应用名，pathList:已安装的是.desktop路径，未安装为空，iconList:已安装的是图标名，未安装的是图标路径
    QStringList nameList, pathList, iconList;
    QVector<QStringList> applist;
    QMap<QString, QList<QString>> appList;
    appList = m_appMatch->startMatchApp(m_keyword);
    QMapIterator<QString,QList<QString>> iter(appList);
    while(iter.hasNext())
    {
        iter.next();
        nameList << iter.key();
        pathList << iter.value().at(0);
        iconList << iter.value().at(1);
    }
    applist.append(nameList);
    applist.append(pathList);
    applist.append(iconList);
    if (!m_stop)
        Q_EMIT this->searchResultApp(applist);
}
