#include "file-search-task.h"
#include "index-status-recorder.h"
#include "dir-watcher.h"
#include "common.h"

#include <QDir>
#include <QFile>
#include <QQueue>
#include <QDebug>

using namespace UkuiSearch;
FileSearchTask::FileSearchTask(QObject *parent)
{
    this->setParent(parent);
    qRegisterMetaType<size_t>("size_t");
    m_pool = new QThreadPool(this);
    m_pool->setMaxThreadCount(1);
}

const QString FileSearchTask::name()
{
    return "File";
}

const QString FileSearchTask::description()
{
    return "File search.";
}

QString FileSearchTask::getCustomSearchType()
{
    return "File";
}

void FileSearchTask::startSearch(std::shared_ptr<SearchController> searchController)
{
    FileSearchWorker *fileSearchWorker;
    fileSearchWorker = new FileSearchWorker(this, searchController);
    m_pool->start(fileSearchWorker);
}

void FileSearchTask::stop()
{

}

void FileSearchTask::sendFinishSignal(size_t searchId)
{
    Q_EMIT searchFinished(searchId);
}


FileSearchWorker::FileSearchWorker(FileSearchTask *fileSarchTask, std::shared_ptr<SearchController> searchController) : m_FileSearchTask(fileSarchTask), m_searchController(searchController)
{
}

void FileSearchWorker::run()
{
    m_currentSearchId = m_searchController->getCurrentSearchId();
    //1.检查是否为不可搜索目录
    QStringList searchDirs = m_searchController->getSearchDir();
    searchDirs.removeDuplicates();

    for (QString &dir : searchDirs) {
        if (dir.endsWith("/")) {
            dir = dir.mid(0, dir.length() - 1);
        }

        QStringList blackListTmp = DirWatcher::getDirWatcher()->blackListOfDir(dir);
        if (!blackListTmp.contains(dir)) {
            m_validDirectories.append(dir);
            m_blackList.append(blackListTmp);
        }
    }

    //TODO 还需要判断是否为不能建立索引的目录
    if (IndexStatusRecorder::getInstance()->indexDatabaseEnable()) {
        qDebug() << "index ready";
        searchWithIndex();

    } else {
        qDebug() << "direct search";
        directSearch();
    }

    QMetaObject::invokeMethod(m_FileSearchTask, "searchFinished", Q_ARG(size_t, m_currentSearchId));
}

Xapian::Query FileSearchWorker::creatQueryForFileSearch() {
    Xapian::Query fileOrDir = Xapian::Query::MatchAll;
    if (!m_searchController->isSearchDirOnly() || !m_searchController->isSearchFileOnly()) {
        if (m_searchController->isSearchDirOnly()) {
            fileOrDir = Xapian::Query(Xapian::Query::OP_VALUE_RANGE, 1, "1", "1");

        } else if (m_searchController->isSearchFileOnly()) {
            fileOrDir = Xapian::Query(Xapian::Query::OP_VALUE_RANGE, 1, "0", "0");
        }

    } else {
        //同时指定只搜索目录和只搜索文件。。。
        return {};
    }

    std::vector<Xapian::Query> queries;
    for (QString &keyword : m_searchController->getKeyword()) {
        if (!keyword.isEmpty()) {
            std::vector<Xapian::Query> queryOfKeyword;
            for(auto &c : keyword) {
                queryOfKeyword.emplace_back(Xapian::Query(QUrl::toPercentEncoding(c).toStdString()));
            }
            queries.emplace_back(Xapian::Query(Xapian::Query::OP_PHRASE, queryOfKeyword.begin(), queryOfKeyword.end()));
        }
    }

    return {Xapian::Query::OP_AND, {Xapian::Query::OP_AND, queries.begin(), queries.end()}, fileOrDir};
}

void FileSearchWorker::searchWithIndex()
{
    try {
        Xapian::Database db(INDEX_PATH.toStdString());
        Xapian::Enquire enquire(db);
//        qDebug() << "===" << QString::fromStdString(creatQueryForFileSearch().get_description());
        enquire.set_query(creatQueryForFileSearch());
        FileSearchFilter fileSearchFilter(this);

        Xapian::MSet result = enquire.get_mset(m_searchController->first(), m_searchController->maxResults(), 0, &fileSearchFilter);

        for (auto it = result.begin(); it != result.end(); ++it) {
            if (m_searchController->beginSearchIdCheck(m_currentSearchId)) {
                std::string data = it.get_document().get_data();

                ResultItem resultItem(m_currentSearchId, QString::fromStdString(data));
                m_searchController->getDataQueue()->enqueue(resultItem);
                m_searchController->finishSearchIdCheck();

            } else {
                qDebug() << "Search id changed!";
                m_searchController->finishSearchIdCheck();
                return;
            }
        }

    } catch(const Xapian::Error &e) {
        qWarning() << QString::fromStdString(e.get_description());
    }
}

void FileSearchWorker::directSearch()
{
    QQueue<QString> searchPathQueue;
    for (QString &dir : m_validDirectories) {
        searchPathQueue.enqueue(dir);
    }

    QDir dir;
    QFileInfoList infoList;
    if (m_searchController->isSearchDirOnly()) {
        dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    } else {
        dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
        dir.setSorting(QDir::DirsFirst);
    }

    while (!searchPathQueue.empty()) {
        dir.setPath(searchPathQueue.dequeue());
        infoList = dir.entryInfoList();

        for (const auto &fileInfo : infoList) {
            if (fileInfo.isDir() && !fileInfo.isSymLink()) {
                QString newPath = fileInfo.absoluteFilePath();
                if (m_blackList.contains(newPath)) {
                    //在黑名单的路径忽略搜索
                    continue;
                }
                if (m_searchController->isRecurse()) {
                    searchPathQueue.enqueue(newPath);
                }
            }

            bool matched = false;
            //同时包含几个key为成功匹配
            for (const QString &keyword: m_searchController->getKeyword()) {
                if (!keyword.isEmpty()) {
                    //TODO 修改匹配方式,对结果进行排序
                    if (fileInfo.fileName().contains(keyword, Qt::CaseInsensitive)) {
                        matched = true;
                        break;
                    }
                }
            }

            if (m_searchController->beginSearchIdCheck(m_currentSearchId)) {
                if (matched) {
                    ResultItem ri(m_currentSearchId, fileInfo.absoluteFilePath());
                    m_searchController->getDataQueue()->enqueue(ri);
                }
                m_searchController->finishSearchIdCheck();
            } else {
                qDebug() << "Search id changed!";
                m_searchController->finishSearchIdCheck();
                return;
            }
        }
    }
}

FileSearchFilter::FileSearchFilter(FileSearchWorker *parent) : parent(parent) {}

bool FileSearchFilter::operator ()(const Xapian::Document &doc) const
{
    if (parent) {
        QString path = QString::fromStdString(doc.get_data());
        bool isRecurse = parent->m_searchController->isRecurse();
        bool inSearchDir = std::any_of(parent->m_validDirectories.begin(), parent->m_validDirectories.end(), [&](QString &dir) {
            bool startWithDir = path.startsWith(dir);
            if (!startWithDir) {
                return false;
            }

            if (path.length() == dir.length()) {
                return false;
            }

            if (!isRecurse) {
                //去除搜索路径后，是否包含 "/"
                return !path.midRef((dir.length() + 1), (path.length() - dir.length() - 1)).contains("/");
            }

            return true;
        });

        bool inBlackList = std::any_of(parent->m_blackList.begin(), parent->m_blackList.end(), [&](QString &dir) {
            return path.startsWith(dir);
        });

        return inSearchDir && !inBlackList;
    }

    return true;
}
