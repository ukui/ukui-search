/*
 * Copyright (C) 2020, KylinSoft Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: zhangpengfei <zhangpengfei@kylinos.cn>
 *
 */
#include "search-manager.h"
using namespace UkuiSearch;
size_t SearchManager::uniqueSymbolFile = 0;
size_t SearchManager::uniqueSymbolDir = 0;
size_t SearchManager::uniqueSymbolContent = 0;
QMutex  SearchManager::m_mutexFile;
QMutex  SearchManager::m_mutexDir;
QMutex  SearchManager::m_mutexContent;
SearchManager::SearchManager(QObject *parent) : QObject(parent) {
    m_pool.setMaxThreadCount(3);
    m_pool.setExpiryTimeout(1000);
}

SearchManager::~SearchManager() {
}

int SearchManager::getCurrentIndexCount() {
    try {
        Xapian::Database db(INDEX_PATH);
        return db.get_doccount();
    } catch(const Xapian::Error &e) {
        qWarning() << QString::fromStdString(e.get_description());
        return 0;
    }
}

void SearchManager::onKeywordSearch(QString keyword, QQueue<QString> *searchResultFile, QQueue<QString> *searchResultDir,
                                    QQueue<QPair<QString, QStringList>> *searchResultContent) {
//    m_mutexFile.lock();
//    ++uniqueSymbolFile;
//    m_mutexFile.unlock();
//    m_mutexDir.lock();
//    ++uniqueSymbolDir;
//    m_mutexDir.unlock();
//    m_mutexContent.lock();
//    ++uniqueSymbolContent;
//    m_mutexContent.unlock();
//    if(FileUtils::SearchMethod::DIRECTSEARCH == FileUtils::searchMethod) {
//        DirectSearch *directSearch;
//        directSearch = new DirectSearch(keyword, searchResultFile, searchResultDir, uniqueSymbolFile);
//        m_pool.start(directSearch);
//    } else if(FileUtils::SearchMethod::INDEXSEARCH == FileUtils::searchMethod) {
//        FileSearch *filesearch;
//        filesearch = new FileSearch(searchResultFile, uniqueSymbolFile, keyword, "0", 1, 0, 5);
//        m_pool.start(filesearch);

//        FileSearch *dirsearch;
//        dirsearch = new FileSearch(searchResultDir, uniqueSymbolDir, keyword, "1", 1, 0, 5);
//        m_pool.start(dirsearch);

//        FileContentSearch *contentSearch;
//        contentSearch = new FileContentSearch(searchResultContent, uniqueSymbolContent, keyword, 0, 5);
//        m_pool.start(contentSearch);
//    } else {
//        qWarning() << "Unknown search method! FileUtils::searchMethod: " << static_cast<int>(FileUtils::searchMethod);
//    }
    return;
}

bool SearchManager::isBlocked(QString &path) {
    QStringList blockList = GlobalSettings::getInstance()->getBlockDirs();
    for(QString i : blockList) {
        if(FileUtils::isOrUnder(path, i))
            return true;
    }
    return false;

}

bool SearchManager::creatResultInfo(SearchPluginIface::ResultInfo &ri, QString path)
{
    QFileInfo info(path);
    if(!info.exists()) {
        return false;
    }
    ri.icon = FileUtils::getFileIcon(QUrl::fromLocalFile(path).toString(), false);
    ri.name = info.fileName();
    ri.description = QVector<SearchPluginIface::DescriptionInfo>() \
                    << SearchPluginIface::DescriptionInfo{tr("Path:"), path} \
                    << SearchPluginIface::DescriptionInfo{tr("Modified time:"), info.lastModified().toString("yyyy/MM/dd hh:mm:ss")};
    ri.actionKey = path;
    ri.type = 0;
    return true;
}
FileSearch::FileSearch(DataQueue<SearchPluginIface::ResultInfo> *searchResult, size_t uniqueSymbol, QString keyword, QString value, unsigned slot, int begin, int num) {
    this->setAutoDelete(true);
    m_search_result = searchResult;
    m_uniqueSymbol = uniqueSymbol;
    m_keyword = keyword;
    m_value = value;
    m_slot = slot;
    m_begin = begin;
    m_num = num;
    m_matchDecider = new FileMatchDecider();
}

FileSearch::~FileSearch() {
    m_search_result = nullptr;
    if(m_matchDecider)
        delete m_matchDecider;
}

void FileSearch::run() {
    if(m_value == "0") {
        SearchManager::m_mutexFile.lock();
        if(!m_search_result->isEmpty()) {
            m_search_result->clear();
        }
        SearchManager::m_mutexFile.unlock();
    } else if(m_value == "1") {
        SearchManager::m_mutexDir.lock();
        if(!m_search_result->isEmpty()) {
            m_search_result->clear();
        }
        SearchManager::m_mutexDir.unlock();
    }
    //目前的需求是文件搜索数量无上限。
    //但如果不设置单次搜索数量限制，在一些性能非常弱的机器上（如兆芯某些机器），就算我们这里不阻塞UI，也会因为搜索本身占用cpu过多（可能）导致UI卡顿。
    //可能会有更好的方法，待优化。
    m_begin = 0;
    m_num = 100;
    int resultCount = 1;
    int totalCount = 0;
    while(resultCount > 0) {
        resultCount = keywordSearchfile();
        m_begin += m_num;
        totalCount += resultCount;
    }
    qDebug() << "Total count:" << m_value << totalCount;
    return;
}

int FileSearch::keywordSearchfile() {
    try {
        qDebug() << "--keywordSearchfile start--";
        Xapian::Database db(INDEX_PATH);
        Xapian::Query query = creatQueryForFileSearch(db);
        Xapian::Enquire enquire(db);

        Xapian::Query queryFile;
        if(!m_value.isEmpty()) {
            std::string slotValue = m_value.toStdString();
            Xapian::Query queryValue = Xapian::Query(Xapian::Query::OP_VALUE_RANGE, m_slot, slotValue, slotValue);
            queryFile = Xapian::Query(Xapian::Query::OP_AND, query, queryValue);
        } else {
            queryFile = query;
        }

        qDebug() << "keywordSearchfile:" << QString::fromStdString(queryFile.get_description());

        enquire.set_query(queryFile);
        Xapian::MSet result = enquire.get_mset(m_begin, m_num, 0, m_matchDecider);
        int resultCount = result.size();
        qDebug() << "keywordSearchfile results count=" << resultCount;
        if(resultCount == 0)
            return 0;
        if(getResult(result) == -1)
            return -1;

        qDebug() << "--keywordSearchfile finish--";
        return resultCount;
    } catch(const Xapian::Error &e) {
        qWarning() << QString::fromStdString(e.get_description());
        qDebug() << "--keywordSearchfile finish--";
        return -1;
    }
}

Xapian::Query FileSearch::creatQueryForFileSearch(Xapian::Database &db) {
    auto userInput = m_keyword.toLower();
    std::vector<Xapian::Query> v;
    for(int i = 0; i < userInput.size(); i++) {
        v.push_back(Xapian::Query(QUrl::toPercentEncoding(userInput.at(i)).toStdString()));
        //        qDebug()<<QString::fromStdString(Xapian::Query(QString(userInput.at(i)).toStdString()).get_description());
    }
    Xapian::Query queryPhrase = Xapian::Query(Xapian::Query::OP_PHRASE, v.begin(), v.end());
    return queryPhrase;
}

int FileSearch::getResult(Xapian::MSet &result) {
    for(auto it = result.begin(); it != result.end(); ++it) {
        Xapian::Document doc = it.get_document();
        std::string data = doc.get_data();
        Xapian::weight docScoreWeight = it.get_weight();
        Xapian::percent docScorePercent = it.get_percent();
        QString path = QString::fromStdString(data);
        std::string().swap(data);

        SearchPluginIface::ResultInfo ri;
        if(SearchManager::creatResultInfo(ri, path)) {
            switch(m_value.toInt()) {
            case 1:
                SearchManager::m_mutexDir.lock();
                if(m_uniqueSymbol == SearchManager::uniqueSymbolDir) {
                    m_search_result->enqueue(ri);
                    SearchManager::m_mutexDir.unlock();
                } else {
                    SearchManager::m_mutexDir.unlock();
                    return -1;
                }

                break;
            case 0:
                SearchManager::m_mutexFile.lock();
                if(m_uniqueSymbol == SearchManager::uniqueSymbolFile) {
                    m_search_result->enqueue(ri);
                    SearchManager::m_mutexFile.unlock();
                } else {
                    SearchManager::m_mutexFile.unlock();
                    return -1;
                }
                break;
            default:
                break;
            }
        }
        //qDebug() << "doc=" << path << ",weight=" << docScoreWeight << ",percent=" << docScorePercent;
    }
    //        if(!pathTobeDelete->isEmpty())
    //            deleteAllIndex(pathTobeDelete)
    return 0;
}

FileContentSearch::FileContentSearch(DataQueue<SearchPluginIface::ResultInfo> *searchResult, size_t uniqueSymbol, QString keyword, int begin, int num) {
    this->setAutoDelete(true);
    m_search_result = searchResult;
    m_uniqueSymbol = uniqueSymbol;
    m_keyword = keyword;
    m_begin = begin;
    m_num = num;
    m_matchDecider = new FileContentMatchDecider();
}

FileContentSearch::~FileContentSearch() {
    m_search_result = nullptr;
    if(m_matchDecider)
        delete m_matchDecider;
}

void FileContentSearch::run() {
    SearchManager::m_mutexContent.lock();
    if(!m_search_result->isEmpty()) {
        m_search_result->clear();
    }
    SearchManager::m_mutexContent.unlock();

    //这里同文件搜索，待优化。
    m_begin = 0;
    m_num = 100;
    int resultCount = 1;
    int totalCount = 0;
    while(resultCount > 0) {
        resultCount = keywordSearchContent();
        m_begin += m_num;
        totalCount += resultCount;
    }
    qDebug() << "Total count:" << totalCount;
    return;
}

int FileContentSearch::keywordSearchContent() {
    try {
        qDebug() << "--keywordSearchContent search start--";

        Xapian::Database db(CONTENT_INDEX_PATH);
        Xapian::Enquire enquire(db);
        Xapian::QueryParser qp;
        qp.set_default_op(Xapian::Query::OP_AND);
        qp.set_database(db);
        /*
                ::friso::ResultMap ret;
                ::friso::FrisoSegmentation::getInstance()->callSegement(ret, keyword.toLocal8Bit().data());
                for (::friso::ResultMap::iterator it_map = ret.begin(); it_map != ret.end(); ++it_map){
                    target_str += it_map->first;
                    target_str += " ";
                    it_map->second.first.clear();
                    ::std::vector<size_t>().swap(it_map->second.first);
                }

                ret.clear();
                ret.erase(ret.begin(), ret.end());
                ::friso::ResultMap().swap(ret);
        */
        QVector<SKeyWord> sKeyWord = ChineseSegmentation::getInstance()->callSegement(m_keyword.toStdString());
        //Creat a query
        std::string words;
        for(int i = 0; i < sKeyWord.size(); i++) {
            words.append(sKeyWord.at(i).word).append(" ");
        }

//        Xapian::Query query = qp.parse_query(words);

        std::vector<Xapian::Query> v;
        for(int i=0; i<sKeyWord.size(); i++) {
            v.push_back(Xapian::Query(sKeyWord.at(i).word));
            qDebug() << QString::fromStdString(sKeyWord.at(i).word);
        }
        Xapian::Query query = Xapian::Query(Xapian::Query::OP_AND, v.begin(), v.end());

        qDebug() << "keywordSearchContent:" << QString::fromStdString(query.get_description());

        enquire.set_query(query);

        Xapian::MSet result = enquire.get_mset(m_begin, m_num, 0, m_matchDecider);
        int resultCount = result.size();
        if(result.size() == 0) {
            return 0;
        }
        qDebug() << "keywordSearchContent results count=" << resultCount;

        if(getResult(result, words) == -1) {
            return -1;
        }

        qDebug() << "--keywordSearchContent search finish--";
        return resultCount;
    } catch(const Xapian::Error &e) {
        qWarning() << QString::fromStdString(e.get_description());
        qDebug() << "--keywordSearchContent search finish--";
        return -1;
    }
}

int FileContentSearch::getResult(Xapian::MSet &result, std::string &keyWord) {
    for(auto it = result.begin(); it != result.end(); ++it) {
        Xapian::Document doc = it.get_document();
        std::string data = doc.get_data();
        double docScoreWeight = it.get_weight();
        Xapian::percent docScorePercent = it.get_percent();
        QString path = QString::fromStdString(doc.get_value(1));

        SearchPluginIface::ResultInfo ri;
        if(!SearchManager::creatResultInfo(ri, path)) {
            continue;
        }
        // Construct snippets containing keyword.
//        QStringList snippets;
//        snippets.append(QString::fromStdString( result.snippet(doc.get_data(),400)));
//        qWarning()<<QString::fromStdString(s);
        auto term = doc.termlist_begin();
        std::string wordTobeFound = QString::fromStdString(keyWord).section(" ", 0, 0).toStdString();
//        int size = wordTobeFound.length();
        term.skip_to(wordTobeFound);
//        int count = 0;
//        for(auto pos = term.positionlist_begin(); pos != term.positionlist_end() && count < 6; ++pos) {
//            std::string s = data.substr((*pos < 60) ? 0 : (*pos  - 60), size + 120);
//            QString snippet = QString::fromStdString(s);
//            if(snippet.size() > 6 + QString::fromStdString(keyWord).size()) {
//                snippet.replace(0, 3, "...").replace(snippet.size() - 3, 3, "...");
//            } else {
//                snippet.append("...").prepend("...");
//            }
//            ri.description.prepend(SearchPluginIface::DescriptionInfo{"",snippet});
////           snippets.append(snippet);
//            QString().swap(snippet);
//            std::string().swap(s);
//            ++count;
//        }
        //fix me: make a snippet without cut cjk char.
        auto pos = term.positionlist_begin();
        QString snippet = FileUtils::chineseSubString(data,*pos,120);

//        if(data.length() - *pos < 120) {
//            std::string s = data.substr((data.length() < 120) ? 0 : (data.length() - 120), 120);
//            snippet = QString::fromStdString(s);
//        } else {
//            std::string s = data.substr(*pos, 120);
//            snippet = QString::fromStdString(s);
//        }

        ri.description.prepend(SearchPluginIface::DescriptionInfo{"",snippet});
        QString().swap(snippet);
//        std::string().swap(s);
        std::string().swap(data);



//        for(QString i : QString::fromStdString(keyWord).split(" ",QString::SkipEmptyParts))
//        {
//            std::string word = i.toStdString();
//            term.skip_to(word);
//            int size = word.size();
//            auto pos = term.positionlist_begin();
//            std::string s = data.substr((*pos < 60)? 0: (*pos  - 60) , size + 120);
//            QString snippet = QString::fromStdString(s);
//            snippet.replace(0,3,"...").replace(snippet.size()-3,3,"...");
//            snippets.append(snippet);
//            std::string().swap(word);
//            std::string().swap(s);
//            QString().swap(snippet);
//        }


        SearchManager::m_mutexContent.lock();
        if(m_uniqueSymbol == SearchManager::uniqueSymbolContent) {
            m_search_result->enqueue(ri);
            SearchManager::m_mutexContent.unlock();
//            snippets.clear();
//            QStringList().swap(snippets);
        } else {
            SearchManager::m_mutexContent.unlock();
            return -1;
        }
        //        searchResult.insert(path,snippets);
        //qDebug() << "path=" << path << ",weight=" << docScoreWeight << ",percent=" << docScorePercent;
    }
//    //        if(!pathTobeDelete->isEmpty())
//    //            deleteAllIndex(pathTobeDelete)
    return 0;
}

DirectSearch::DirectSearch(QString keyword, DataQueue<SearchPluginIface::ResultInfo> *searchResult, QString value, size_t uniqueSymbol) {
    this->setAutoDelete(true);
    m_keyword = keyword;
    m_searchResult = searchResult;
    m_uniqueSymbol = uniqueSymbol;
    m_value = value;
}

void DirectSearch::run() {
    QStringList blockList = GlobalSettings::getInstance()->getBlockDirs();
    if(blockList.contains(QStandardPaths::writableLocation(QStandardPaths::HomeLocation).remove(0,1), Qt::CaseSensitive)) {
        return;
    }
    QQueue<QString> bfs;
    bfs.enqueue(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    QFileInfoList list;
    QDir dir;
    // QDir::Hidden
    if(m_value == DIR_SEARCH_VALUE) {
         dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    } else {
        dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
        dir.setSorting(QDir::DirsFirst);
    }
    while(!bfs.empty()) {
        dir.setPath(bfs.dequeue());
        list = dir.entryInfoList();
        for (auto i : list) {
            if (i.isDir() && (!(i.isSymLink()))) {
                bool findIndex = false;
                for (QString j : blockList) {
                    if (FileUtils::isOrUnder(i.absoluteFilePath(), j)) {
                        findIndex = true;
                        break;
                    }
                }
                if (findIndex == true) {
                    qDebug() << "path is blocked:" << i.absoluteFilePath();
                    continue;
                }
                bfs.enqueue(i.absoluteFilePath());
            }
            if(i.fileName().contains(m_keyword, Qt::CaseInsensitive)) {
//                qWarning() << i.fileName() << m_keyword;
                if(m_searchResult->length() > 49)
                    return;
                if((i.isDir() && m_value == DIR_SEARCH_VALUE)) {
                    SearchPluginIface::ResultInfo ri;
                    if(SearchManager::creatResultInfo(ri,i.absoluteFilePath())) {
                        SearchManager::m_mutexDir.lock();
                        if(m_uniqueSymbol == SearchManager::uniqueSymbolDir) {
                            m_searchResult->enqueue(ri);
                            SearchManager::m_mutexDir.unlock();
                        } else {
                            SearchManager::m_mutexDir.unlock();
                            return;
                        }
                    }
                } else if (i.isFile() && m_value == FILE_SEARCH_VALUE) {
                    SearchPluginIface::ResultInfo ri;
                    if(SearchManager::creatResultInfo(ri,i.absoluteFilePath())) {
                        SearchManager::m_mutexFile.lock();
                        if(m_uniqueSymbol == SearchManager::uniqueSymbolFile) {
                            m_searchResult->enqueue(ri);
                            SearchManager::m_mutexFile.unlock();
                        } else {
                            SearchManager::m_mutexFile.unlock();
                            return;
                        }
                    }
                }
            }
        }
    }
}

bool FileMatchDecider::operator ()(const Xapian::Document &doc) const
{
    QString path = QString::fromStdString(doc.get_data());
    if(SearchManager::isBlocked(path)) {
        return false;
    }
    return true;
}

bool FileContentMatchDecider::operator ()(const Xapian::Document &doc) const
{
    QString path = QString::fromStdString(doc.get_value(1));
    if(SearchManager::isBlocked(path)) {
        return false;
    }
    return true;
}
