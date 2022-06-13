//
// Created by hxf on 2022/4/18.
//

//ukui-search
#include "file-content-search-task.h"
#include "index-status-recorder.h"
#include "chinese-segmentation.h"
#include "result-item.h"
#include "common.h"
#include "file-utils.h"

//Qt
#include <QIcon>
#include <QDebug>
#include <utility>

using namespace UkuiSearch;

FileContentSearchTask::FileContentSearchTask(QObject *parent)
{
    this->setParent(parent);
    qRegisterMetaType<size_t>("size_t");
    m_pool = new QThreadPool(this);
    m_pool->setMaxThreadCount(1);
}

FileContentSearchTask::~FileContentSearchTask()
{

}

PluginInterface::PluginType FileContentSearchTask::pluginType()
{
    return PluginType::SearchTaskPlugin;
}

const QString FileContentSearchTask::name()
{
    return tr("File Content");
}

const QString FileContentSearchTask::description()
{
    return tr("File Content Search");
}

const QIcon FileContentSearchTask::icon()
{
    return QIcon::fromTheme("folder", QIcon(":/unknown.svg"));
}

void FileContentSearchTask::setEnable(bool enable)
{
    m_enable = enable;
}

bool FileContentSearchTask::isEnable()
{
    return m_enable && IndexStatusRecorder::getInstance()->contentIndexDatabaseEnable();
}

QString FileContentSearchTask::getCustomSearchType()
{
    return tr("File Content");
}

SearchType FileContentSearchTask::getSearchType()
{
    return SearchType::FileContent;
}

void FileContentSearchTask::startSearch(std::shared_ptr<SearchController> searchController)
{
    FileContentSearchWorker *worker = new FileContentSearchWorker(this, searchController);
    m_pool->start(worker);
}

void FileContentSearchTask::stop()
{

}

FileContentSearchWorker::FileContentSearchWorker(FileContentSearchTask *fileContentSearchTask, std::shared_ptr<SearchController> searchController)
{
    m_fileContentSearchTask = fileContentSearchTask;
    m_searchController = std::move(searchController);
    m_currentSearchId = m_searchController->getCurrentSearchId();
}

void FileContentSearchWorker::run()
{
    QStringList searchDirs = m_searchController->getSearchDir();
    searchDirs.removeDuplicates();

    for (QString &dir : searchDirs) {
        if (dir.endsWith("/")) {
            dir = dir.mid(0, dir.length() - 1);
        }

        //TODO 指定黑名单
        m_validDirectories.append(dir);
    }

    bool finished = true;
    if (IndexStatusRecorder::getInstance()->contentIndexDatabaseEnable()) {
        qDebug() << "content index ready";
        finished = execSearch();

    } else {
        qWarning() << "content index incomplete";
        sendErrorMsg(QObject::tr("Content index incomplete."));

        finished = false;
    }

    if (finished) QMetaObject::invokeMethod(m_fileContentSearchTask, "searchFinished", Q_ARG(size_t, m_currentSearchId));
}

void FileContentSearchWorker::sendErrorMsg(const QString &msg)
{
    QMetaObject::invokeMethod(m_fileContentSearchTask, "searchError",
                              Q_ARG(size_t, m_currentSearchId),
                              Q_ARG(QString, msg));
}

bool FileContentSearchWorker::execSearch()
{
    try {
        Xapian::Database db(CONTENT_INDEX_PATH.toStdString());
        Xapian::Enquire enquire(db);

        enquire.set_query(createQuery());

        FileContentSearchFilter filter(this);

        Xapian::MSet result = enquire.get_mset(m_searchController->first(), m_searchController->maxResults(), 0, &filter);

        for (auto it = result.begin(); it != result.end(); ++it) {
            if (m_searchController->beginSearchIdCheck(m_currentSearchId)) {
                QString path = QString::fromStdString(it.get_document().get_value(CONTENT_DATABASE_PATH_SLOT));

                ResultItem resultItem(m_currentSearchId, path);
                m_searchController->getDataQueue()->enqueue(resultItem);
                m_searchController->finishSearchIdCheck();

            } else {
                qDebug() << "Search id changed!";
                m_searchController->finishSearchIdCheck();
                return false;
            }
        }

        return true;

    } catch (const Xapian::Error &e) {
        qWarning() << QString::fromStdString(e.get_description());
        sendErrorMsg("Xapian Error: " + QString::fromStdString(e.get_description()));
        return false;
    }
}

inline Xapian::Query FileContentSearchWorker::createQuery()
{
    std::vector<Xapian::Query> v;

    for (const auto &keyword : m_searchController->getKeyword()) {
        std::vector<KeyWord> sKeyWord = ChineseSegmentation::getInstance()->callSegment(keyword.toStdString());

        for(const auto & c : sKeyWord) {
            v.emplace_back(c.word);
        }
    }

    return {Xapian::Query::OP_AND, v.begin(), v.end()};
}


FileContentSearchFilter::FileContentSearchFilter(FileContentSearchWorker *worker) : m_worker(worker)
{

}

bool FileContentSearchFilter::operator()(const Xapian::Document &doc) const
{
    //在此处对搜索结果进行过滤
    QString path = QString::fromStdString(doc.get_value(CONTENT_DATABASE_PATH_SLOT));
    bool isExists = QFileInfo::exists(path);
    bool inSearchDir = true;

    //如果不指定搜索目录，那么搜索整个数据库
    if (m_worker && !m_worker->m_validDirectories.empty()) {
        inSearchDir = std::any_of(m_worker->m_validDirectories.begin(), m_worker->m_validDirectories.end(), [=] (const QString& dir) {
            return path.startsWith(dir);
        });
        //TODO 黑名单
    }

    return isExists && inSearchDir;
}
