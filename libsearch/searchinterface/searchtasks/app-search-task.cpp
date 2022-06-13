#include "app-search-task.h"
#include "index-status-recorder.h"
#include "common.h"
#include <qt5xdg/XdgIcon>
#include <QDir>
#include <QFile>
#include <QQueue>
#include <QDebug>

using namespace UkuiSearch;
AppSearchTask::AppSearchTask(QObject *parent)
{
    this->setParent(parent);
    qRegisterMetaType<size_t>("size_t");
    m_pool = new QThreadPool(this);
    m_pool->setMaxThreadCount(1);
}

const QString AppSearchTask::name()
{
    return tr("Application");
}

const QString AppSearchTask::description()
{
    return tr("Application search.");
}

QString AppSearchTask::getCustomSearchType()
{
    return "Application";
}

void AppSearchTask::startSearch(std::shared_ptr<SearchController> searchController)
{
    AppSearchWorker *appSearchWorker;
    appSearchWorker = new AppSearchWorker(this, searchController);
    m_pool->start(appSearchWorker);
}

void AppSearchTask::stop()
{

}

void AppSearchTask::sendFinishSignal(size_t searchId)
{
    Q_EMIT searchFinished(searchId);
}


AppSearchWorker::AppSearchWorker(AppSearchTask *AppSarchTask, std::shared_ptr<SearchController> searchController) : m_AppSearchTask(AppSarchTask), m_searchController(searchController)
{
    qDBusRegisterMetaType<QMap<QString, QString>>();
    qDBusRegisterMetaType<QList<QMap<QString, QString>>>();
    m_interFace = new QDBusInterface("com.kylin.softwarecenter.getsearchresults", "/com/kylin/softwarecenter/getsearchresults",
                                     "com.kylin.getsearchresults",
                                     QDBusConnection::sessionBus());
    if(!m_interFace->isValid()) {
        qWarning() << qPrintable(QDBusConnection::sessionBus().lastError().message());
    }
    m_interFace->setTimeout(1500);
}

void AppSearchWorker::run()
{
    m_currentSearchId = m_searchController->getCurrentSearchId();
    bool finished = true;
    QStringList results;
    QStringList keyWords = m_searchController->getKeyword();
    ResultDataTypes dataType = m_searchController->getResultDataType(SearchType::Application);
    m_appInfoTable.searchInstallApp(keyWords, results);
    for (int i = 0; i < results.size() / 3; i++) {
        if (m_searchController->beginSearchIdCheck(m_currentSearchId)) {
            QVariantList info;
            if (dataType & UkuiSearch::ApplicationDesktopPath) {
                info << QVariant(results.at(i*3));
            }
            if (dataType & UkuiSearch::ApplicationLocalName) {
                info << QVariant(results.at(i*3 + 1));
            }
            if (dataType & UkuiSearch::ApplicationIconName) {
                info << QVariant(XdgIcon::fromTheme(results.at(i*3 + 2)));
            }
            if (dataType & UkuiSearch::ApplicationDescription) {//本地应用暂无简介
                info << QVariant(QString());
            }
            if (dataType & UkuiSearch::IsOnlineApplication) {
                info << QVariant(0);
            }
            ResultItem ri(m_currentSearchId, results.at(i*3), info);
            m_searchController->getDataQueue()->enqueue(ri);
            m_searchController->finishSearchIdCheck();
        } else {
            qDebug() << "Search id changed!";
            m_searchController->finishSearchIdCheck();
        }
    }
    if (m_searchController->isSearchOnlineApps()) {
        //online app search
        for (auto keyword : keyWords) {
            QDBusReply<QList<QMap<QString, QString>>> reply = m_interFace->call("get_search_result", keyword); //阻塞，直到远程方法调用完成。
            if(reply.isValid()) {
                for(int i = 0; i < reply.value().size(); i++) {
                    if (m_searchController->beginSearchIdCheck(m_currentSearchId)) {
                        QVariantList info;
                        if (dataType & UkuiSearch::ApplicationDesktopPath) {
                            info << QVariant(reply.value().at(i).value("appname"));
                        }
                        if (dataType & UkuiSearch::ApplicationLocalName) {
                            QLocale locale;
                            if(locale.language() == QLocale::Chinese) {
                                info << QVariant(reply.value().at(i).value("displayname_cn"));
                            } else {
                                info << QVariant(reply.value().at(i).value("appname"));
                            }
                        }
                        if (dataType & UkuiSearch::ApplicationIconName) {
                            info << QVariant(QIcon(reply.value().at(i).value("icon")));
                        }
                        if (dataType & UkuiSearch::ApplicationDescription) {//在线应用有效
                            info << QVariant(reply.value().at(i).value("discription"));
                        }
                        if (dataType & UkuiSearch::IsOnlineApplication) {
                            info << QVariant(1);
                        }
                        ResultItem ri(m_currentSearchId, reply.value().at(i).value("appname"), info);
                        m_searchController->getDataQueue()->enqueue(ri);
                        m_searchController->finishSearchIdCheck();
                    } else {
                        qDebug() << "Search id changed!";
                        m_searchController->finishSearchIdCheck();
                    }
                }
            } else {
                qWarning() << "SoftWareCenter dbus called failed!";
            }
        }
    }

    if (finished) QMetaObject::invokeMethod(m_AppSearchTask, "searchFinished", Q_ARG(size_t, m_currentSearchId));
}

AppSearchWorker::~AppSearchWorker()
{

}

void AppSearchWorker::sendErrorMsg(const QString &msg)
{
    QMetaObject::invokeMethod(m_AppSearchTask, "searchError",
                              Q_ARG(size_t, m_currentSearchId),
                              Q_ARG(QString, msg));
}
