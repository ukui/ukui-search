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
#include <QFileInfo>
#include <QDebug>
#include <QtConcurrent>
#include <QThread>
#include <QUrl>
#include "search-manager.h"
#include "global-settings.h"

size_t SearchManager::uniqueSymbol1 = 0;
size_t SearchManager::uniqueSymbol2 = 0;
size_t SearchManager::uniqueSymbol3 = 0;
QMutex  SearchManager::m_mutex1;
QMutex  SearchManager::m_mutex2;
QMutex  SearchManager::m_mutex3;
SearchManager::SearchManager(QObject *parent) : QObject(parent)
{
    m_pool.setMaxThreadCount(2);
    m_pool.setExpiryTimeout(1000);
}

SearchManager::~SearchManager()
{
}

int SearchManager::getCurrentIndexCount()
{
    try
    {
        Xapian::Database db(INDEX_PATH);
        return db.get_doccount();
    }
    catch(const Xapian::Error &e)
    {
        qWarning() <<QString::fromStdString(e.get_description());
        return 0;
    }
}

void SearchManager::onKeywordSearch(QString keyword,QQueue<QString> *searchResultFile,QQueue<QString> *searchResultDir,
                                    QQueue<QPair<QString,QStringList>> *searchResultContent)
{
    m_mutex1.lock();
    ++uniqueSymbol1;
    m_mutex1.unlock();
    m_mutex2.lock();
    ++uniqueSymbol2;
    m_mutex2.unlock();
    m_mutex3.lock();
    ++uniqueSymbol3;
    m_mutex3.unlock();

    FileSearch *filesearch;
    filesearch = new FileSearch(searchResultFile,uniqueSymbol1,keyword,"0",1,0,5);
    m_pool.start(filesearch);

    FileSearch *dirsearch;
    dirsearch = new FileSearch(searchResultDir,uniqueSymbol2,keyword,"1",1,0,5);
    m_pool.start(dirsearch);

    FileContentSearch *contentSearch;
    contentSearch = new FileContentSearch(searchResultContent,uniqueSymbol3,keyword,0,5);
    m_pool.start(contentSearch);
    return;
}

bool SearchManager::isBlocked(QString &path)
{
    QStringList blockList = GlobalSettings::getInstance()->getBlockDirs();
    for(QString i : blockList)
    {
        if(path.startsWith(i.prepend("/")))
            return true;
    }
    return false;

}

FileSearch::FileSearch(QQueue<QString> *searchResult,size_t uniqueSymbol, QString keyword, QString value, unsigned slot, int begin, int num)
{
    this->setAutoDelete(true);
    m_search_result = searchResult;
    m_uniqueSymbol = uniqueSymbol;
    m_keyword = keyword;
    m_value = value;
    m_slot = slot;
    m_begin = begin;
    m_num = num;
}

FileSearch::~FileSearch()
{
    m_search_result = nullptr;
}

void FileSearch::run()
{
    if(!m_search_result->isEmpty())
        m_search_result->clear();
    int resultCount = 0;
    int total = 0;
    while(total < 100)
    {
        resultCount = keywordSearchfile();
        if(resultCount == 0 || resultCount == -1)
            break;
        total += resultCount;
        m_begin += m_num;
    }
    return;
}

int FileSearch::keywordSearchfile()
{
    try
    {
        qDebug() << "--keywordSearchfile start--";
        Xapian::Database db(INDEX_PATH);
        Xapian::Query query = creatQueryForFileSearch(db);
        Xapian::Enquire enquire(db);

        Xapian::Query queryFile;
        if(!m_value.isEmpty())
        {
            std::string slotValue = m_value.toStdString();
            Xapian::Query queryValue = Xapian::Query(Xapian::Query::OP_VALUE_RANGE,m_slot,slotValue,slotValue);
            queryFile = Xapian::Query(Xapian::Query::OP_AND,query,queryValue);
        }
        else
        {
            queryFile = query;
        }

        qDebug() << "keywordSearchfile:"<<QString::fromStdString(queryFile.get_description());

        enquire.set_query(queryFile);
        Xapian::MSet result = enquire.get_mset(m_begin, m_num);
        int resultCount = result.size();
        qDebug()<< "keywordSearchfile results count=" <<resultCount;
        if(resultCount == 0)
            return 0;
        if(getResult(result) == -1)
            return -1;

        qDebug()<< "--keywordSearchfile finish--";
        return resultCount;
    }
    catch(const Xapian::Error &e)
    {
        qWarning() <<QString::fromStdString(e.get_description());
        qDebug()<< "--keywordSearchfile finish--";
        return -1;
    }
}

Xapian::Query FileSearch::creatQueryForFileSearch(Xapian::Database &db)
{
    auto userInput = m_keyword.toLower();
    std::vector<Xapian::Query> v;
    for(int i=0;i<userInput.size();i++)
    {
        v.push_back(Xapian::Query(QUrl::toPercentEncoding(userInput.at(i)).toStdString()));
        //        qDebug()<<QString::fromStdString(Xapian::Query(QString(userInput.at(i)).toStdString()).get_description());
    }
    Xapian::Query queryPhrase =Xapian::Query(Xapian::Query::OP_PHRASE, v.begin(), v.end());
    return queryPhrase;
}

int FileSearch::getResult(Xapian::MSet &result)
{
    for (auto it = result.begin(); it != result.end(); ++it)
    {
        Xapian::Document doc = it.get_document();
        std::string data = doc.get_data();
        Xapian::weight docScoreWeight = it.get_weight();
        Xapian::percent docScorePercent = it.get_percent();
        QString path = QString::fromStdString(data);
        std::string().swap(data);

        if(SearchManager::isBlocked(path))
            continue;

        QFileInfo info(path);

        if(!info.exists())
        {
            //                pathTobeDelete->append(QString::fromStdString(data));
            qDebug()<<path<<"is not exist!!";
        }
        else
        {
            switch (m_value.toInt())
            {
            case 1:
                SearchManager::m_mutex1.lock();
                if(m_uniqueSymbol == SearchManager::uniqueSymbol2)
                {
                    m_search_result->enqueue(path);
                    SearchManager::m_mutex1.unlock();
                }
                else
                {
                    SearchManager::m_mutex1.unlock();
                    return -1;
                }

                break;
            case 0:
                SearchManager::m_mutex2.lock();
                if(m_uniqueSymbol == SearchManager::uniqueSymbol1)
                {
                    m_search_result->enqueue(path);
                    SearchManager::m_mutex2.unlock();
                }
                else
                {
                    SearchManager::m_mutex2.unlock();
                    return -1;
                }
                break;
            default:
                break;
            }
            //            searchResult.append(path);
        }
        qDebug()<< "doc="<< path << ",weight=" <<docScoreWeight << ",percent=" << docScorePercent;
    }
    //        if(!pathTobeDelete->isEmpty())
    //            deleteAllIndex(pathTobeDelete)
    return 0;
}

FileContentSearch::FileContentSearch(QQueue<QPair<QString,QStringList>> *searchResult, size_t uniqueSymbol, QString keyword, int begin, int num)
{
    this->setAutoDelete(true);
    m_search_result = searchResult;
    m_uniqueSymbol = uniqueSymbol;
    m_keyword = keyword;
    m_begin = begin;
    m_num = num;
}

FileContentSearch::~FileContentSearch()
{
    m_search_result = nullptr;
}

void FileContentSearch::run()
{
    if(!m_search_result->isEmpty())
        m_search_result->clear();
    int resultCount = 0;
    int total = 0;

    while(total<50)
    {
        resultCount = keywordSearchContent();
        if(resultCount == 0 || resultCount == -1)
            break;
        total += resultCount;
        m_begin += m_num;
    }
    return;
}

int FileContentSearch::keywordSearchContent()
{
    try
    {
        qDebug()<<"--keywordSearchContent search start--";

        Xapian::Database db(CONTENT_INDEX_PATH);
        Xapian::Enquire enquire(db);
        Xapian::QueryParser qp;
        qp.set_default_op(Xapian::Query::OP_AND);
        qp.set_database(db);


        ::std::string target_str;
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
        FrisoUtils::segementOnlyInSearch(target_str, m_keyword.toLocal8Bit().data());

        Xapian::Query query = qp.parse_query(target_str);
//        Xapian::Query query = qp.parse_query(keyword.toStdString());



//        QVector<SKeyWord> sKeyWord = ChineseSegmentation::getInstance()->callSegement(keyword);
//        //Creat a query
//        std::string words;
//        for(int i=0;i<sKeyWord.size();i++)
//        {
//            words.append(sKeyWord.at(i).word).append(" ");
//        }


//        Xapian::Query query = qp.parse_query(words);

        //        std::vector<Xapian::Query> v;
        //        for(int i=0;i<sKeyWord.size();i++)
        //        {
        //            v.push_back(Xapian::Query(sKeyWord.at(i).word));
        //            qDebug()<<QString::fromStdString(sKeyWord.at(i).word);
        //        }
        //        Xapian::Query queryPhrase =Xapian::Query(Xapian::Query::OP_AND, v.begin(), v.end());
        qDebug()<<"keywordSearchContent:"<<QString::fromStdString(query.get_description());

        enquire.set_query(query);

        Xapian::MSet result = enquire.get_mset(m_begin, m_num);
        int resultCount = result.size();
        if(result.size() == 0)
            return 0;
        qDebug()<< "keywordSearchContent results count=" <<resultCount;

        if(getResult(result,target_str) == -1)
            return -1;

        qDebug()<< "--keywordSearchContent search finish--";
        return resultCount;
    }
    catch(const Xapian::Error &e)
    {
        qWarning() <<QString::fromStdString(e.get_description());
        qDebug()<< "--keywordSearchContent search finish--";
        return -1;
    }
}

int FileContentSearch::getResult(Xapian::MSet &result, std::string &keyWord)
{
    for (auto it = result.begin(); it != result.end(); ++it)
    {
        Xapian::Document doc = it.get_document();
        std::string data = doc.get_data();
        double docScoreWeight = it.get_weight();
        Xapian::percent docScorePercent = it.get_percent();
        QString path = QString::fromStdString(doc.get_value(1));

        if(SearchManager::isBlocked(path))
            continue;

        QFileInfo info(path);

        if(!info.exists())
        {
            //                pathTobeDelete->append(QString::fromStdString(data));
            qDebug()<<path<<"is not exist!!";
            continue;
        }
        // Construct snippets containing keyword.
        QStringList snippets;
//        snippets.append(QString::fromStdString( result.snippet(doc.get_data(),400)));
//        qWarning()<<QString::fromStdString(s);
        auto term = doc.termlist_begin();
        std::string wordTobeFound = QString::fromStdString(keyWord).section(" ",0,0).toStdString();
        int size = wordTobeFound.length();
        term.skip_to(wordTobeFound);
        int count =0;
        for(auto pos = term.positionlist_begin();pos != term.positionlist_end()&&count < 6;++pos)
        {
            std::string s = data.substr((*pos < 60)? 0: (*pos  - 60) , size + 120);
            QString snippet = QString::fromStdString(s);
            snippet.replace(0,3,"...").replace(snippet.size()-3,3,"...");
            snippets.append(snippet);
            QString().swap(snippet);
            std::string().swap(s);
            ++count;
        }
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

        SearchManager::m_mutex3.lock();
        if(m_uniqueSymbol == SearchManager::uniqueSymbol3)
        {

            m_search_result->enqueue(qMakePair(path,snippets));
            SearchManager::m_mutex3.unlock();
            snippets.clear();
            QStringList().swap(snippets);
        }
        else
        {
            SearchManager::m_mutex3.unlock();
            return -1;
        }
        //        searchResult.insert(path,snippets);
        qDebug()<< "path="<< path << ",weight=" <<docScoreWeight << ",percent=" << docScorePercent;
    }
//    //        if(!pathTobeDelete->isEmpty())
//    //            deleteAllIndex(pathTobeDelete)
    return 0;
}
