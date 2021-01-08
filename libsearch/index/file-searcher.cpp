#include <QFileInfo>
#include <QDebug>
#include <QtConcurrent>
#include <chinese-segmentation.h>
#include "file-searcher.h"
#include "global-settings.h"


FileSearcher::FileSearcher(QObject *parent) : QObject(parent)
{
}

void FileSearcher::onKeywordSearch(QString keyword)
{
    //file
    QtConcurrent::run([=](){
        int begin = 0;
        int num = 20;
        int resultCount = 0;
        while(1)
        {
            resultCount = keywordSearchfile(keyword,"1",1,begin,num);
            if(resultCount == 0 || resultCount == -1)
                break;
            begin += num;
        }
    });
    Q_EMIT this->resultFile(m_search_result_file);
    //dir
    QtConcurrent::run([=](){
        int begin = 0;
        int num = 20;
        int resultCount = 0;
        while(1)
        {
            resultCount = keywordSearchfile(keyword,"0",1,begin,num);
            if(resultCount == 0 || resultCount == -1)
                break;
            begin += num;
        }
    });
    Q_EMIT this->resultDir(m_search_result_dir);
    //content
    QtConcurrent::run([=](){
        int begin = 0;
        int num = 10;
        int resultCount = 0;
        while(1)
        {
            keywordSearchContent(keyword,begin,num);
            if(resultCount == 0 || resultCount == -1)
                break;
            begin += num;
        }
    });
    Q_EMIT this->resultContent(m_search_result_content);
}

int FileSearcher::keywordSearchfile(QString keyword, QString value, unsigned slot, int begin, int num)
{
    try
    {
        qDebug()<<"--search start--";
        Xapian::Database db(INDEX_PATH);
        Xapian::Query query = creatQueryForFileSearch(keyword,db);
        Xapian::Enquire enquire(db);

        Xapian::Query queryFile;
        if(!value.isEmpty())
        {
            std::string slotValue = value.toStdString();
            Xapian::Query queryValue = Xapian::Query(Xapian::Query::OP_VALUE_RANGE,slot,slotValue,slotValue);
            queryFile = Xapian::Query(Xapian::Query::OP_AND,query,queryValue);
        }
        else
        {
            queryFile = query;
        }

        qDebug()<<QString::fromStdString(queryFile.get_description());

        enquire.set_query(queryFile);
        Xapian::MSet result = enquire.get_mset(begin, begin+num);
        int resultCount =  static_cast<int>(result.get_matches_estimated());
        qDebug()<< "find results count=" <<resultCount;
        getResult(result,value);

        qDebug()<< "--search finish--";
        return resultCount;
    }
    catch(const Xapian::Error &e)
    {
        qWarning() <<QString::fromStdString(e.get_description());
        qDebug()<< "--search finish--";
        return -1;
    }
}

int FileSearcher::keywordSearchContent(QString keyword, int begin, int num)
{
    try
    {
        qDebug()<<"--content search start--";

        Xapian::Database db(CONTENT_INDEX_PATH);
        Xapian::Enquire enquire(db);
        Xapian::QueryParser qp;
        qp.set_default_op(Xapian::Query::OP_AND);
        qp.set_database(db);

        QVector<SKeyWord> sKeyWord = ChineseSegmentation::getInstance()->callSegement(&keyword);
        //Creat a query
        std::string words;
        for(int i=0;i<sKeyWord.size();i++)
        {
            words.append(sKeyWord.at(i).word).append(" ");
        }
        Xapian::Query query = qp.parse_query(words);

//        std::vector<Xapian::Query> v;
//        for(int i=0;i<sKeyWord.size();i++)
//        {
//            v.push_back(Xapian::Query(sKeyWord.at(i).word));
//            qDebug()<<QString::fromStdString(sKeyWord.at(i).word);
//        }
//        Xapian::Query queryPhrase =Xapian::Query(Xapian::Query::OP_AND, v.begin(), v.end());
        qDebug()<<QString::fromStdString(query.get_description());

        enquire.set_query(query);
        //dir result
        Xapian::MSet result = enquire.get_mset(begin, begin+num);
        int resultCount = static_cast<int>(result.get_matches_estimated());
        qDebug()<< "find results count=" <<resultCount;

        getContentResult(result,words);

        qDebug()<< "--content search finish--";
        return resultCount;
    }
    catch(const Xapian::Error &e)
    {
        qWarning() <<QString::fromStdString(e.get_description());
        qDebug()<< "--content search finish--";
        return -1;
    }
}
Xapian::Query FileSearcher::creatQueryForFileSearch(QString keyword, Xapian::Database &db)
{
    Xapian::QueryParser qp;
    qp.set_default_op(Xapian::Query::OP_PHRASE);
    qp.set_database(db);
    auto userInput = keyword;

    std::string queryStr = keyword.replace(""," ").toStdString();
//        std::string s =db.get_spelling_suggestion(queryStr,10);
//        qDebug()<<"spelling_suggestion!"<<QString::fromStdString(s);

    qDebug()<<"queryStr!"<<QString::fromStdString(queryStr);
    //Creat a query
    Xapian::Query queryPhrase = qp.parse_query(queryStr,Xapian::QueryParser::FLAG_PHRASE);
    std::vector<Xapian::Query> v;
    for(int i=0;i<userInput.size();i++)
    {
        v.push_back(Xapian::Query(QString(userInput.at(i)).toStdString()));
        qDebug()<<QString::fromStdString(Xapian::Query(QString(userInput.at(i)).toStdString()).get_description());
    }
    Xapian::Query queryNear =Xapian::Query(Xapian::Query::OP_NEAR, v.begin(), v.end());

    return Xapian::Query(Xapian::Query::OP_AND,queryNear,queryPhrase);
}

Xapian::Query FileSearcher::creatQueryForContentSearch(QString keyword, Xapian::Database &db)
{

}

QStringList FileSearcher::getResult(Xapian::MSet &result, QString value)
{
    //QStringList *pathTobeDelete = new QStringList;
    //Delete those path doc which is not already exist.

    QStringList searchResult = QStringList();
    if(result.size() == 0)
        return searchResult;

    for (auto it = result.begin(); it != result.end(); ++it)
    {
        Xapian::Document doc = it.get_document();
        std::string data = doc.get_data();
        Xapian::weight docScoreWeight = it.get_weight();
        Xapian::percent docScorePercent = it.get_percent();
        QString path = QString::fromStdString(data);

        if(isBlocked(path))
            continue;

        QFileInfo *info = new QFileInfo(path);

        if(!info->exists())
        {
//                pathTobeDelete->append(QString::fromStdString(data));
            qDebug()<<path<<"is not exist!!";
        }
        else
        {
            switch (value.toInt())
            {
            case 1:
                m_search_result_dir->enqueue(path);
                break;
            case 0:
                m_search_result_file->enqueue(path);
                break;
            default:
                break;
            }
            searchResult.append(path);
        }
        qDebug()<< "doc="<< path << ",weight=" <<docScoreWeight << ",percent=" << docScorePercent;
    }
    //        if(!pathTobeDelete->isEmpty())
    //            deleteAllIndex(pathTobeDelete)
    return searchResult;
}

QMap<QString,QStringList> FileSearcher::getContentResult(Xapian::MSet &result, std::string &keyWord)
{
    //QStringList *pathTobeDelete = new QStringList;
    //Delete those path doc which is not already exist.

    QString wordTobeFound = QString::fromStdString(keyWord).section(" ",0,0);
    int size = wordTobeFound.size();
    int totalSize = QString::fromStdString(keyWord).size();
    if(totalSize < 5)
        totalSize = 5;
    QMap<QString,QStringList> searchResult;
    if(result.size() == 0)
        return searchResult;

    for (auto it = result.begin(); it != result.end(); ++it)
    {
        Xapian::Document doc = it.get_document();
        std::string data = doc.get_data();
        double docScoreWeight = it.get_weight();
        Xapian::percent docScorePercent = it.get_percent();
        QString path = QString::fromStdString(doc.get_value(1));

        if(isBlocked(path))
            continue;

        QFileInfo *info = new QFileInfo(path);

        if(!info->exists())
        {
            //                pathTobeDelete->append(QString::fromStdString(data));
            qDebug()<<path<<"is not exist!!";
            continue;
        }
        // Construct snippets containing keyword.
        QStringList snippets;
        auto term = doc.termlist_begin();
        term.skip_to(wordTobeFound.toStdString());
        int count =0;
        for(auto pos = term.positionlist_begin();pos != term.positionlist_end()&&count < 6;++pos)
        {
            QByteArray snippetByte = QByteArray::fromStdString(data);
            QString snippet = "..."+QString(snippetByte.left(*pos)).right(size +totalSize) + QString(snippetByte.mid(*pos,-1)).left(size+totalSize) + "...";
//            qDebug()<<snippet;
            snippets.append(snippet);
            ++count;
        }
        m_search_result_content->enqueue(qMakePair(path,snippets));
        searchResult.insert(path,snippets);
        qDebug()<< "path="<< path << ",weight=" <<docScoreWeight << ",percent=" << docScorePercent;
    }
    //        if(!pathTobeDelete->isEmpty())
    //            deleteAllIndex(pathTobeDelete)
    return searchResult;
}

bool FileSearcher::isBlocked(QString &path)
{
    QStringList blockList = GlobalSettings::getInstance()->getBlockDirs();
    for(QString i :blockList)
    {
        if(path.startsWith(i))
            return true;
    }
    return false;

}
