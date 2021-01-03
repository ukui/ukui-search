#include "file-searcher.h"
#include <QFileInfo>
#include <QDebug>
#include <chinese-segmentation.h>

FileSearcher::FileSearcher(QObject *parent) : QObject(parent)
{

}

void FileSearcher::onKeywordSearch(QString keyword, int begin, int num)
{
    QVector<QStringList> searchResult;
    try
    {
        qDebug()<<"--search start--";

        Xapian::Database db(INDEX_PATH);
        Xapian::Enquire enquire(db);
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
            qDebug()<<userInput.at(i);
            qDebug()<<QString::fromStdString(Xapian::Query(QString(userInput.at(i)).toStdString()).get_description());
        }
        Xapian::Query queryNear =Xapian::Query(Xapian::Query::OP_NEAR, v.begin(), v.end());
        Xapian::Query query = Xapian::Query(Xapian::Query::OP_AND,queryNear,queryPhrase);

        //1- dir 2-file
        unsigned slot = 1;
        std::string value = "1";
        Xapian::Query queryValue1 = Xapian::Query(Xapian::Query::OP_VALUE_GE,slot,value);
        value = "0";
        Xapian::Query queryValue0 = Xapian::Query(Xapian::Query::OP_VALUE_LE,1,value);
        Xapian::Query queryDir = Xapian::Query(Xapian::Query::OP_AND,query,queryValue1);
        Xapian::Query queryFile = Xapian::Query(Xapian::Query::OP_AND,query,queryValue0);

        qDebug()<<QString::fromStdString(query.get_description());

        enquire.set_query(queryDir);
        //dir result
        Xapian::MSet result = enquire.get_mset(begin, begin+num);
        qDebug()<< "find dir results count=" <<static_cast<int>(result.get_matches_estimated());
        searchResult.append(getResult(result));

        enquire.set_query(queryFile);
        //file result
        result = enquire.get_mset(begin, begin+num);
        qDebug()<< "find file results count=" <<static_cast<int>(result.get_matches_estimated());
        searchResult.append(getResult(result));

        qDebug()<< "--search finish--";
    }
    catch(const Xapian::Error &e)
    {
        qDebug() <<QString::fromStdString(e.get_description());
        return;
    }
    Q_EMIT this->result(searchResult);
    return;

}

void FileSearcher::onKeywordSearchContent(QString keyword, int begin, int num)
{
    QMap<QString,QStringList> searchResult = QMap<QString,QStringList>();
    try
    {
        qDebug()<<"--content search start--";

        Xapian::Database db(CONTENT_INDEX_PATH);
        Xapian::Enquire enquire(db);
        Xapian::QueryParser qp;
        qp.set_default_op(Xapian::Query::OP_AND);
        qp.set_database(db);

        QVector<SKeyWord> sKeyWord = ChineseSegmentation::callSegement(&keyword);
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
        qDebug()<< "find results count=" <<static_cast<int>(result.get_matches_estimated());

        searchResult = getContentResult(result,words);

        qDebug()<< "--content search finish--";
    }
    catch(const Xapian::Error &e)
    {
        qDebug() <<QString::fromStdString(e.get_description());
        qDebug()<< "--content search finish--";
        return;
    }
    Q_EMIT this->contentResult(searchResult);
//    qDebug()<<searchResult;
    return;
}

QStringList FileSearcher::getResult(Xapian::MSet &result)
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
        QFileInfo *info = new QFileInfo(QString::fromStdString(data));

        if(!info->exists())
        {
//                pathTobeDelete->append(QString::fromStdString(data));
            qDebug()<<QString::fromStdString(data)<<"is not exist!!";
        }
        else
        {
            searchResult.append(QString::fromStdString(data));
        }
        qDebug()<< "doc="<< QString::fromStdString(data) << ",weight=" <<docScoreWeight << ",percent=" << docScorePercent;
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
        searchResult.insert(path,snippets);
        qDebug()<< "path="<< path << ",weight=" <<docScoreWeight << ",percent=" << docScorePercent;
    }
    //        if(!pathTobeDelete->isEmpty())
    //            deleteAllIndex(pathTobeDelete)
    return searchResult;
}
