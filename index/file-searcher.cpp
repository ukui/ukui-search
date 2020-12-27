#include "file-searcher.h"
#include <QFileInfo>
#include <QDebug>

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
        qDebug()<<"value!!!!"<<QString::fromStdString(doc.get_value(1));
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
