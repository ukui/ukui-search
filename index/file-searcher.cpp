#include "file-searcher.h"

FileSearcher::FileSearcher(QObject *parent) : QObject(parent)
{

}

void FileSearcher::onKeywordSearch(QString keyword)
{
    QStringList searchResult;
    try
    {
        qDebug()<<"--search start--";

        Xapian::Database db(INDEX_PATH);
        Xapian::Enquire enquire(db);
        Xapian::QueryParser qp;
        qp.set_default_op(Xapian::Query::OP_PHRASE);
        qp.set_database(db);
        auto userInput = keyword;

        std::string queryStr = indexText.replace(""," ").toStdString();
//        std::string s =db.get_spelling_suggestion(queryStr,10);
//        qDebug()<<"spelling_suggestion!"<<QString::fromStdString(s);

        qDebug()<<"queryStr!"<<QString::fromStdString(queryStr);
        //Creat a query
        Xapian::Query queryPhrase = qp.parse_query(queryStr,Xapian::QueryParser::FLAG_PHRASE);
        vector<Xapian::Query> v;
        for(int i=0;i<userInput.size();i++)
        {
            v.push_back(Xapian::Query(QString(userInput.at(i)).toStdString()));
            qDebug()<<userInput.at(i);
            qDebug()<<QString::fromStdString(Xapian::Query(QString(userInput.at(i)).toStdString()).get_description());
        }
        Xapian::Query queryNear =Xapian::Query(Xapian::Query::OP_NEAR, v.begin(), v.end());
        Xapian::Query query = Xapian::Query(Xapian::Query::OP_AND,queryNear,queryPhrase);

        qDebug()<<QString::fromStdString(query.get_description());
        enquire.set_query(query);

        Xapian::MSet result = enquire.get_mset(0, 9999);
        qDebug()<< "find results count=" <<static_cast<int>(result.get_matches_estimated());

//        QStringList *pathTobeDelete = new QStringList;

        //get search result
        for (auto it = result.begin(); it != result.end(); ++it) {
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
//        //Delete those path doc which is not already exist.
//        if(!pathTobeDelete->isEmpty())
//            deleteAllIndex(pathTobeDelete);

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
