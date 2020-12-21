#include "index-generator.h"
#include <QFile>
#include <QFileInfo>
#include <QDebug>

using namespace std;

#define INDEX_PATH "/home/zpf/.config/org.ukui/index_data"
static IndexGenerator *global_instance = nullptr;

IndexGenerator *IndexGenerator::getInstance()
{
    if (!global_instance) {
        global_instance = new IndexGenerator;
    }
    return global_instance;
}

bool IndexGenerator::creatAllIndex(QStringList *pathlist)
{
    HandlePathList(pathlist);
    try
    {
        m_indexer = new Xapian::TermGenerator();
        m_indexer->set_database(*m_datebase);
        m_indexer->set_flags(Xapian::TermGenerator::FLAG_SPELLING);
        m_indexer->set_stemming_strategy(Xapian::TermGenerator::STEM_SOME);

        QMap<QString, QString>::const_iterator i;
        QString *indexStr;
        QString *docStr;
        for(i=m_index_map->constBegin();i!=m_index_map->constEnd();++i)
        {
            docStr = new QString(i.key());
            indexStr = new QString(i.value());
            CreatIndex(indexStr,docStr);
        }
        m_datebase->commit();
    }
    catch(const Xapian::Error &e)
    {
        qDebug()<<"creatAllIndex fail!"<<QString::fromStdString(e.get_description());
        return false;
    }
    m_index_map->clear();
    Q_EMIT this->transactionFinished();

    return true;
}

IndexGenerator::IndexGenerator(QObject *parent) : QObject(parent)
{
    m_datebase = new Xapian::WritableDatabase(INDEX_PATH, Xapian::DB_CREATE_OR_OPEN);
    m_cryp = new QCryptographicHash(QCryptographicHash::Md5);
}

IndexGenerator::~IndexGenerator()
{
}

void IndexGenerator::CreatIndex(QString *indexText,QString *doc)
{
    qDebug()<< "--index start--";
    m_docstr = doc->toStdString();
    m_index_text_str = indexText->toStdString();

    std::string uniqueterm = m_cryp->hash(doc->toUtf8(),QCryptographicHash::Md5).toStdString();

    Xapian::Document document;
    document.set_data(m_docstr);
    document.add_term(uniqueterm);
    m_indexer->set_document(document);
    m_indexer->index_text(m_index_text_str);

    Xapian::docid innerId= m_datebase->replace_document(uniqueterm,document);

    //        qDebug()<<"replace doc uniqueterm="<<QString::fromStdString(uniqueterm);
    qDebug()<<"replace doc docid="<<static_cast<int>(innerId);
    qDebug()<< "--index finish--";
    return;

}

void IndexGenerator::HandlePathList(QStringList *pathlist)
{
    qDebug()<<"Begin HandlePathList!";
    m_index_map = new QMap<QString,QString>;
    QStringList *list = pathlist;
    for(int i = 0;i<list->size();i++)
    {
        auto info = new QFileInfo(list->at(i));
        //提取文件名并用空格分割,同时去除'.'
        QString filename = info->fileName();
        QString index_test = filename.replace(".","").replace(""," ");
        index_test.simplified();
        //            qDebug()<<"index_test"<<index_test;
        m_index_map->insert(info->absoluteFilePath(),index_test);
        //            qDebug()<<m_index_map->value(index_test);
    }

    qDebug()<<"Finish HandlePathList!";
    return;
}

bool IndexGenerator::isIndexdataExist()
{

//    Xapian::Database db(m_index_data_path->toStdString());


}

void IndexGenerator::IndexSearch(QString *indexText)
{
    try
    {
        qDebug()<<"--search start--";

        Xapian::Enquire enquire(*m_datebase);
        Xapian::QueryParser qp;
        qp.set_default_op(Xapian::Query::OP_NEAR);
        qp.set_database(*m_datebase);

        std::string queryStr = indexText->replace(""," ").toStdString();
//        std::string s =db.get_spelling_suggestion(queryStr,10);
//        qDebug()<<"spelling_suggestion!"<<QString::fromStdString(s);

        qDebug()<<"queryStr!"<<QString::fromStdString(queryStr);
        Xapian::Query query = qp.parse_query(queryStr);;
        qDebug()<<QString::fromStdString(query.get_description());
        enquire.set_query(query);

        Xapian::MSet result = enquire.get_mset(0, 9999);
        qDebug()<< "find results count=" <<static_cast<int>(result.get_matches_estimated());

        QStringList *pathTobeDelete = new QStringList;
        for (auto it = result.begin(); it != result.end(); ++it) {
            Xapian::Document doc = it.get_document();
            std::string data = doc.get_data();
            Xapian::weight docScoreWeight = it.get_weight();
            Xapian::percent docScorePercent = it.get_percent();
            QFileInfo *info = new QFileInfo(QString::fromStdString(data));

            if(!info->exists())
            {
                pathTobeDelete->append(QString::fromStdString(data));
                qDebug()<<QString::fromStdString(data)<<"is not exist!!";
            }

            qDebug()<< "doc="<< QString::fromStdString(data) << ",weight=" <<docScoreWeight << ",percent=" << docScorePercent;
        }
        //Delete those path doc which is not already exist.
        if(!pathTobeDelete->isEmpty())
            deleteAllIndex(pathTobeDelete);

        qDebug()<< "--search finish--";
    }
    catch(const Xapian::Error &e)
    {
        qDebug() <<QString::fromStdString(e.get_description());
    }
    Q_EMIT this->searchFinish();
    return;
}

bool IndexGenerator::deleteAllIndex(QStringList *pathlist)
{
    QStringList *list = pathlist;
    if(list->isEmpty())
        return true;
    for(int i = 0;i<list->size();i++)
    {
        QString doc = list->at(i);
        std::string uniqueterm = m_cryp->hash(doc.toUtf8(),QCryptographicHash::Md5).toStdString();;
        try
        {
            qDebug()<<"--delete start--";
            m_datebase->delete_document(uniqueterm);
            qDebug()<<"delete md5"<<QString::fromStdString(uniqueterm);
            m_datebase->commit();
            qDebug()<< "--delete finish--";
        }
        catch(const Xapian::Error &e)
        {
            qDebug() <<QString::fromStdString(e.get_description());
            return false;
        }
    }
    Q_EMIT this->transactionFinished();
    return true;
}

