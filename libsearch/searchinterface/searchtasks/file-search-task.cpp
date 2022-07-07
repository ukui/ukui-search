#include "file-search-task.h"
#include "index-status-recorder.h"
#include "dir-watcher.h"
#include "common.h"

#include <QDir>
#include <QUrl>
#include <QFile>
#include <QQueue>
#include <QDebug>
#include <gio/gio.h>

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
        if (QFileInfo::exists(dir)) {
            if (dir.endsWith("/")) {
                dir.remove(dir.length() - 1, 1);
            }

            QStringList blackListTmp = DirWatcher::getDirWatcher()->blackListOfDir(dir);
            if (!blackListTmp.contains(dir)) {
                m_validDirectories.append(dir);
                m_blackList.append(blackListTmp);
            }
        }
    }

    //过滤空标签
    for (QString &label : m_searchController->getFileLabel()) {
        if (!label.isEmpty()) {
            m_labels.append(label);
        }
    }

    bool finished = true;
    //TODO 还需要判断是否为不能建立索引的目录
    if (IndexStatusRecorder::getInstance()->indexDatabaseEnable()) {
        qDebug() << "index ready";
        finished = searchWithIndex();

    } else {
        if (m_validDirectories.empty()) {
            //TODO 使用全局的默认可搜索目录
            if (QFileInfo::exists(QDir::homePath())) {
                sendErrorMsg(QObject::tr("Warning, Can not find home path."));
                return;
            }
            m_validDirectories.append(QDir::homePath());
            m_blackList.append(DirWatcher::getDirWatcher()->blackListOfDir(QDir::homePath()));
        }
        qDebug() << "direct search";
        finished = directSearch();
    }

    if (finished) QMetaObject::invokeMethod(m_FileSearchTask, "searchFinished", Q_ARG(size_t, m_currentSearchId));
}

Xapian::Query FileSearchWorker::creatQueryForFileSearch() {
    Xapian::Query fileOrDir = Xapian::Query::MatchAll;
    if (m_searchController->isSearchDirOnly() && m_searchController->isSearchFileOnly()) {
        //同时指定'只搜索目录'和'只搜索文件',那麼拒絕搜索
        return {};

    } else {
        if (m_searchController->isSearchDirOnly()) {
            fileOrDir = Xapian::Query(Xapian::Query::OP_VALUE_RANGE, 1, "1", "1");

        } else if (m_searchController->isSearchFileOnly()) {
            fileOrDir = Xapian::Query(Xapian::Query::OP_VALUE_RANGE, 1, "0", "0");
        }
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

bool FileSearchWorker::searchWithIndex()
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
                return false;
            }
        }

    } catch(const Xapian::Error &e) {
        qWarning() << QString::fromStdString(e.get_description());
        sendErrorMsg("Xapian Error: " + QString::fromStdString(e.get_description()));
        return false;
    }

    return true;
}

void FileSearchWorker::sendErrorMsg(const QString &msg)
{
    QMetaObject::invokeMethod(m_FileSearchTask, "searchError",
                              Q_ARG(size_t, m_currentSearchId),
                              Q_ARG(QString, msg));
}

bool FileSearchWorker::directSearch()
{
    unsigned int maxResults = m_searchController->maxResults();
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

                bool inBlackList = std::any_of(m_blackList.begin(), m_blackList.end(), [&] (const QString &dir) {
                    return newPath.startsWith(dir + "/");
                });

                if (inBlackList) {
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

            if (matched && !m_labels.empty()) {
                matched = FileSearchFilter::checkFileLabel(fileInfo.absoluteFilePath(), m_labels);
            }

            if (m_searchController->beginSearchIdCheck(m_currentSearchId)) {
                if (matched) {
                    ResultItem ri(m_currentSearchId, fileInfo.absoluteFilePath());
                    m_searchController->getDataQueue()->enqueue(ri);
                    --maxResults;
                }
                m_searchController->finishSearchIdCheck();
                if (maxResults == 0) {
                    return true;
                }
            } else {
                qDebug() << "Search id changed!";
                m_searchController->finishSearchIdCheck();
                return false;
            }
        }
    }

    return true;
}

FileSearchFilter::FileSearchFilter(FileSearchWorker *parent) : m_parent(parent) {}

bool FileSearchFilter::operator ()(const Xapian::Document &doc) const
{
    if (m_parent) {
        QString path = QString::fromStdString(doc.get_data());
        bool isRecurse = m_parent->m_searchController->isRecurse();
        bool inSearchDir = true;

        if (!m_parent->m_validDirectories.empty()) {
            inSearchDir = std::any_of(m_parent->m_validDirectories.begin(), m_parent->m_validDirectories.end(), [&](QString &dir) {
                //限制搜索在该目录下
                if (!path.startsWith(dir + "/")) {
                    return false;
                }

                if (!isRecurse) {
                    //去除搜索路径后，是否包含 "/"
                    return !path.midRef((dir.length() + 1), (path.length() - dir.length() - 1)).contains("/");
                }

                return true;
            });
        }

        bool hasLabel = true;
        if (inSearchDir && !m_parent->m_labels.empty()) {
            hasLabel = FileSearchFilter::checkFileLabel(path, m_parent->m_labels);
        }

        return inSearchDir && hasLabel;
    }

    return true;
}

bool FileSearchFilter::checkFileLabel(const QString &path, const QStringList &labels)
{
    bool hasLabel = false;

    GFile *file = g_file_new_for_path(path.toUtf8().constData());
    if (file) {
        GFileInfo *info = g_file_query_info(file, "metadata::*," G_FILE_ATTRIBUTE_ID_FILE, G_FILE_QUERY_INFO_NONE,
                                            nullptr, nullptr);
        if (info) {
            gboolean hasKey = g_file_info_has_attribute(info, "metadata::peony-file-label-ids");
            if (hasKey) {
                char *fileLabels = g_file_info_get_attribute_as_string(info, "metadata::peony-file-label-ids");
                if (fileLabels) {
                    QStringList fileLabelList = QString::fromUtf8(fileLabels).split("\n");
                    g_free(fileLabels);

                    hasLabel = std::any_of(fileLabelList.begin(), fileLabelList.end(), [&](QString &fileLabel) {
                        return labels.contains(fileLabel);
                    });
                }
            }

            g_object_unref(info);
        }

        g_object_unref(file);
    }

    return hasLabel;
}
