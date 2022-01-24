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
 * Modified by: zhangzihao <zhangzihao@kylinos.cn>
 *
 */
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QtConcurrent>
#include <QFuture>
#include <QThreadPool>
#include <QFile>
#include "file-utils.h"
#include "index-generator.h"
#include "chinese-segmentation.h"
#include <QStandardPaths>
#include <malloc.h>

#define INDEX_PATH (QStandardPaths::writableLocation(QStandardPaths::HomeLocation)+"/.config/org.ukui/ukui-search/index_data").toStdString()
#define CONTENT_INDEX_PATH (QStandardPaths::writableLocation(QStandardPaths::HomeLocation)+"/.config/org.ukui/ukui-search/content_index_data").toStdString()
#define OCR_INDEX_PATH (QStandardPaths::writableLocation(QStandardPaths::HomeLocation)+"/.config/org.ukui/ukui-search/ocr_index_data").toStdString()

using namespace Zeeker;

static IndexGenerator *global_instance = nullptr;
QMutex  IndexGenerator::m_mutex;
//QVector<Document> *Zeeker::g_docListForPath;
//QMutex  Zeeker::g_mutexDocListForPath;
//QVector<Document> *Zeeker::g_docListForContent;
//QMutex  Zeeker::g_mutexDocListForContent;
QMutex  IndexGenerator::g_mutexDocListForPath;
QMutex  IndexGenerator::g_mutexDocListForContent;
QMutex  IndexGenerator::g_mutexDocListForOcr;
QVector<Document> IndexGenerator::g_docListForPath = QVector<Document>();
QVector<Document> IndexGenerator::g_docListForContent = QVector<Document>();
QVector<Document> IndexGenerator::g_docListForOcr = QVector<Document>();


IndexGenerator *IndexGenerator::getInstance(bool rebuild, QObject *parent) {
    QMutexLocker locker(&m_mutex);
    if(!global_instance) {
        qDebug() << "IndexGenerator=================";
        global_instance = new IndexGenerator(rebuild, parent);
    }
    qDebug() << "global_instance" << global_instance;
    qDebug() << "QThread::currentThreadId()" << QThread::currentThreadId();
    return global_instance;
}

bool IndexGenerator::setIndexdataPath() {
    return true;
}

//文件名索引
bool IndexGenerator::creatAllIndex(QQueue<QVector<QString> > *messageList) {
    HandlePathList(messageList);
//    if(g_docListForPath == NULL) {
//        return false;
//    }
    if(IndexGenerator::g_docListForPath.isEmpty()) {
        return false;
    }
    qDebug() << "begin creatAllIndex";
    try {
        for(auto i : IndexGenerator::g_docListForPath) {

            insertIntoDatabase(i);
        }
        m_database_path->commit();
    } catch(const Xapian::Error &e) {
        qWarning() << "creatAllIndex fail!" << QString::fromStdString(e.get_description());
        //need a record
        IndexStatusRecorder::getInstance()->setStatus(INDEX_DATABASE_STATE, "1");
        assert(false);
    }
    qDebug() << "finish creatAllIndex";
    IndexGenerator::g_docListForPath.clear();
    IndexGenerator::g_docListForPath.squeeze();
    QVector<Document>().swap(IndexGenerator::g_docListForPath);

//    delete g_docListForPath;
//    g_docListForPath = nullptr;
    return true;
}
//文件内容索引
bool IndexGenerator::creatAllIndex(QQueue<QString> *messageList) {
    HandlePathList(messageList);
    qDebug() << "begin creatAllIndex for content";
    if(IndexGenerator::g_docListForContent.isEmpty()) {
        return false;
    }
    int size = IndexGenerator::g_docListForContent.size();
    qDebug() << "begin creatAllIndex for content" << size;
    if(!size == 0) {
        try {
            int count = 0;
            for(Document i : IndexGenerator::g_docListForContent) {
                if(!i.isRequiredDeleted()) {
                    m_database_content->replace_document(i.getUniqueTerm(), i.getXapianDocument());
                } else {
                    m_database_content->delete_document(i.getUniqueTerm());
                }
                if(++count > 999) {
                    count = 0;
                    m_database_content->commit();
                }
            }
            m_database_content->commit();
        } catch(const Xapian::Error &e) {
            qWarning() << "creat content Index fail!" << QString::fromStdString(e.get_description());
            IndexStatusRecorder::getInstance()->setStatus(CONTENT_INDEX_DATABASE_STATE, "1");
            assert(false);
        }
        qDebug() << "finish creatAllIndex for content";

        IndexGenerator::g_docListForContent.clear();
        IndexGenerator::g_docListForContent.squeeze();
        QVector<Document>().swap(IndexGenerator::g_docListForContent);
        malloc_trim(0);
    }
    Q_EMIT this->transactionFinished();
    return true;

}

bool IndexGenerator::creatOcrIndex(QQueue<QString> *messageList)
{
    HandleOcrPathList(messageList);
    if(IndexGenerator::g_docListForOcr.isEmpty()) {
        return false;
    }
    int size = IndexGenerator::g_docListForOcr.size();
    qDebug() << "begin creatAllIndex for ocr" << size;
    if(!size == 0) {
        try {
            int count = 0;
            for(Document i : IndexGenerator::g_docListForOcr) {
                if(!i.isRequiredDeleted()) {
                    m_database_ocr->replace_document(i.getUniqueTerm(), i.getXapianDocument());
                } else {
                    m_database_ocr->delete_document(i.getUniqueTerm());
                }
                if(++count > 999) {
                    count = 0;
                    m_database_ocr->commit();
                }
            }
            m_database_ocr->commit();
        } catch(const Xapian::Error &e) {
            qWarning() << "creat ocr Index fail!" << QString::fromStdString(e.get_description());
            IndexStatusRecorder::getInstance()->setStatus(CONTENT_INDEX_DATABASE_STATE, "1");
            assert(false);
        }
        qDebug() << "finish creatAllIndex for ocr";

        IndexGenerator::g_docListForOcr.clear();
        IndexGenerator::g_docListForOcr.squeeze();
        QVector<Document>().swap(IndexGenerator::g_docListForOcr);
        malloc_trim(0);
    }
    return true;
}

IndexGenerator::IndexGenerator(bool rebuild, QObject *parent) : QObject(parent) {
    QDir database(QString::fromStdString(INDEX_PATH));

    if(database.exists()) {
        if(rebuild)
            qDebug() << "remove" << database.removeRecursively();
    } else {
        qDebug() << "create index path" << database.mkpath(QString::fromStdString(INDEX_PATH));
    }
    database.setPath(QString::fromStdString(CONTENT_INDEX_PATH));
    if(database.exists()) {
        if(rebuild)
            qDebug() << "remove" << database.removeRecursively();
    } else {
        qDebug() << "create content index path" << database.mkpath(QString::fromStdString(CONTENT_INDEX_PATH));
    }

    m_database_path = new Xapian::WritableDatabase(INDEX_PATH, Xapian::DB_CREATE_OR_OPEN);
    m_database_content = new Xapian::WritableDatabase(CONTENT_INDEX_PATH, Xapian::DB_CREATE_OR_OPEN);
    m_database_ocr = new Xapian::WritableDatabase(OCR_INDEX_PATH, Xapian::DB_CREATE_OR_OPEN);
}

IndexGenerator::~IndexGenerator() {
    QMutexLocker locker(&m_mutex);
    qDebug() << "~IndexGenerator";
    if(m_database_path)
        m_database_path->~WritableDatabase();
//        delete m_database_path;
    m_database_path = nullptr;
    if(m_database_content)
        m_database_content->~WritableDatabase();
//        delete m_database_content;
    if(m_database_ocr)
        m_database_ocr->~WritableDatabase();
    m_database_path = nullptr;
    m_database_content = nullptr;
    m_database_ocr = nullptr;
    global_instance = nullptr;
//    if(m_index_map)
//        delete m_index_map;
//    m_index_map = nullptr;
//    if(mg_docListForPath)
//        delete mg_docListForPath;
//    mg_docListForPath = nullptr;
//    if(mg_docListForContent)
//        delete mg_docListForContent;
//    mg_docListForContent = nullptr;
//    if(m_index_data_path)
//        delete m_index_data_path;
//    m_index_data_path = nullptr;
//    if(m_indexer)
//        delete m_indexer;
//    m_indexer = nullptr;
//    GlobalSettings::getInstance()->setValue(INDEX_DATABASE_STATE, "2");
//    GlobalSettings::getInstance()->setValue(CONTENT_INDEX_DATABASE_STATE, "2");
//    GlobalSettings::getInstance()->setValue(INDEX_GENERATOR_NORMAL_EXIT, "2");

    qDebug() << "QThread::currentThreadId()" << QThread::currentThreadId();
    qDebug() << "~IndexGenerator end";
}

void IndexGenerator::insertIntoDatabase(Document& doc) {
//    qDebug()<< "--index start--";
    Xapian::Document document = doc.getXapianDocument();
//    m_indexer.set_document(document);
//    qDebug()<<doc.getIndexText();

//    for(auto i : doc.getIndexText()){
//        m_indexer.index_text(i.toStdString());
//    }
//    qDebug()<<"insert m_database_path:"<<QString::fromStdString(document.get_data());
//    qDebug()<<"document:"<<QString::fromStdString(document.get_description());

    Xapian::docid innerId = m_database_path->replace_document(doc.getUniqueTerm(), document);
//    qDebug()<<"replace doc docid="<<static_cast<int>(innerId);
//    qDebug()<< "--index finish--";
    return;
}
//#define fun(a) a=new ;printf()
void IndexGenerator::insertIntoContentDatabase(Document& doc) {
    Xapian::docid innerId = m_database_content->replace_document(doc.getUniqueTerm(), doc.getXapianDocument());
//    qDebug()<<"replace doc docid="<<static_cast<int>(innerId);
//    qDebug()<< "--index finish--";
    return;
}

void IndexGenerator::HandlePathList(QQueue<QVector<QString>> *messageList) {
    qDebug() << "Begin HandlePathList!";
    qDebug() << messageList->size();
//    qDebug()<<QString::number(quintptr(QThread::currentThreadId()));
//    QFuture<Document> future = QtConcurrent::mapped(*messageList,&IndexGenerator::GenerateDocument);

//    future.waitForFinished();

//    QList<Document> docList = future.results();
//    future.cancel();
//    mg_docListForPath = new QList<Document>(docList);
    QThreadPool pool;
    pool.setMaxThreadCount(((QThread::idealThreadCount() - 1) / 2) + 1);
    pool.setExpiryTimeout(100);
    ConstructDocumentForPath *constructer;
    while(!messageList->isEmpty()) {
        constructer = new ConstructDocumentForPath(messageList->dequeue());
        pool.start(constructer);
    }
    qDebug() << "pool finish" << pool.waitForDone(-1);
//    if(constructer)
//        delete constructer;
//    constructer = nullptr;

//    qDebug()<<g_docListForPath->size();
//    qWarning() << g_docListForPath;
//    QList<Document> docList = future.results();
//    mg_docListForPath = new QList<Document>(docList);
//    mg_docListForPath = std::move(future.results());
//    qDebug()<<mg_docListForPath.size();

    qDebug() << "Finish HandlePathList!";
    return;
}

void IndexGenerator::HandlePathList(QQueue<QString> *messageList) {
    qDebug() << "Begin HandlePathList for content index!";
    qDebug() << messageList->size();
//    qDebug()<<QString::number(quintptr(QThread::currentThreadId()));
    ChineseSegmentation::getInstance();
    ConstructDocumentForContent *constructer;
    QThreadPool pool;
//    pool.setMaxThreadCount(((QThread::idealThreadCount() - 1) / 2) + 1);
    pool.setMaxThreadCount(1);
    pool.setExpiryTimeout(100);
    while(!messageList->isEmpty()) {
        constructer = new ConstructDocumentForContent(messageList->dequeue());
        pool.start(constructer);
    }
    qDebug() << "pool finish" << pool.waitForDone(-1);
    qDebug() << "Finish HandlePathList for content index!";
    return;
}

void IndexGenerator::HandleOcrPathList(QQueue<QString> *messageList)
{
    qDebug() << "Begin HandlePathList for ocr index!";
    qDebug() << messageList->size();
    ConstructDocumentForOcr *constructer;
    QThreadPool pool;
    pool.setMaxThreadCount(1);
    pool.setExpiryTimeout(100);
    while(!messageList->isEmpty()) {
        constructer = new ConstructDocumentForOcr(messageList->dequeue());
        pool.start(constructer);
    }
    qDebug() << "pool finish" << pool.waitForDone(-1);
    qDebug() << "Finish HandlePathList for content index!";
    return;
}
//deprecated
Document IndexGenerator::GenerateDocument(const QVector<QString> &list) {
    Document doc;
//    qDebug()<<QString::number(quintptr(QThread::currentThreadId()));
    //0-filename 1-filepathname 2-file or dir
    QString index_text = list.at(0);
    QString sourcePath = list.at(1);
    index_text = index_text.replace("", " ");
    index_text = index_text.simplified();

    //不带多音字版
//    QString pinyin_text = FileUtils::find(QString(list.at(0)).replace(".","")).replace("", " ").simplified();

    //多音字版
    //现加入首字母
    QStringList pinyin_text_list = FileUtils::findMultiToneWords(QString(list.at(0)).replace(".", ""));
    for(QString& i : pinyin_text_list) {
        i.replace("", " ");
        i = i.simplified();
    }

    QString uniqueterm = QString::fromStdString(FileUtils::makeDocUterm(sourcePath));
    QString upTerm = QString::fromStdString(FileUtils::makeDocUterm(sourcePath.section("/", 0, -2, QString::SectionIncludeLeadingSep)));
//    QString uniqueterm1 = QString::fromStdString(QCryptographicHash::hash(sourcePath.toUtf8(),QCryptographicHash::Md5).toStdString());
    /*--------------------------------------------------------------------*/
    //QByteArray 和  QString 之间会进行隐式转换，造成字符串被截断等意想不到的后果！！！！！！！ zpf
    //    if(uniqueterm1!=uniqueterm){
//        qDebug()<<"-----------------------------------------start";
//        qDebug()<<uniqueterm1;
//        qDebug()<<uniqueterm;
//        qDebug()<<"------------------------------------------finish";
//    }
    /*--------------------------------------------------------------------*/

    doc.setData(sourcePath);
    doc.setUniqueTerm(uniqueterm);
    doc.addTerm(upTerm);
    doc.addValue(1, list.at(2));
    QStringList temp;
    temp.append(index_text);
    temp.append(pinyin_text_list);
    doc.setIndexText(temp);
    return doc;

}
//deprecated
Document IndexGenerator::GenerateContentDocument(const QString &path) {
//    构造文本索引的document
    QString content;
    QStringList tmp;
    QVector<SKeyWord> term;
    SKeyWord skw;
    Document doc;
    QString uniqueterm;
    QString upTerm;
    FileReader::getTextContent(path, content);

    term = ChineseSegmentation::getInstance()->callSegement(content.toStdString());
//    QStringList  term = content.split("");

    doc.setData(content);
    doc.setUniqueTerm(uniqueterm);
    doc.addTerm(upTerm);
    doc.addValue(1, path);
    for(int i = 0; i < term.size(); ++i) {
        doc.addPosting(term.at(i).word, term.at(i).offsets, static_cast<int>(term.at(i).weight));

    }

//    Document doc;
//        doc.setData(content);
//        doc.setUniqueTerm(uniqueterm);
//        doc.addTerm(upTerm);
//        doc.addValue(path);
//        int pos = 0;
//        for(QString i : term)
//        {
//            doc.addPosting(i.toStdString(),QVector<size_t>() << ++pos,1);
//        }

    content.clear();
    term.clear();
    return doc;
}

bool IndexGenerator::isIndexdataExist() {

//    Xapian::Database db(m_index_data_path->toStdString());
    return true;


}
//deprecated
QStringList IndexGenerator::IndexSearch(QString indexText) {
    QStringList searchResult;
    try {
        qDebug() << "--search start--";

        Xapian::Database db(INDEX_PATH);
        Xapian::Enquire enquire(db);
        Xapian::QueryParser qp;
        qp.set_default_op(Xapian::Query::OP_PHRASE);
        qp.set_database(db);
        auto userInput = indexText;

        std::string queryStr = indexText.replace("", " ").toStdString();
//        std::string s =db.get_spelling_suggestion(queryStr,10);
//        qDebug()<<"spelling_suggestion!"<<QString::fromStdString(s);

        qDebug() << "queryStr!" << QString::fromStdString(queryStr);
        //Creat a query
        Xapian::Query queryPhrase = qp.parse_query(queryStr, Xapian::QueryParser::FLAG_PHRASE);
        std::vector<Xapian::Query> v;
        for(int i = 0; i < userInput.size(); i++) {
            v.push_back(Xapian::Query(QString(userInput.at(i)).toStdString()));
            qDebug() << userInput.at(i);
            qDebug() << QString::fromStdString(Xapian::Query(QString(userInput.at(i)).toStdString()).get_description());
        }
        Xapian::Query queryNear = Xapian::Query(Xapian::Query::OP_NEAR, v.begin(), v.end());
        Xapian::Query query = Xapian::Query(Xapian::Query::OP_AND, queryNear, queryPhrase);

        qDebug() << QString::fromStdString(query.get_description());
        enquire.set_query(query);

        Xapian::MSet result = enquire.get_mset(0, 9999);
        qDebug() << "find results count=" << static_cast<int>(result.get_matches_estimated());

//        QStringList *pathTobeDelete = new QStringList;

        //get search result
        for(auto it = result.begin(); it != result.end(); ++it) {
            Xapian::Document doc = it.get_document();
            std::string data = doc.get_data();
            Xapian::weight docScoreWeight = it.get_weight();
            Xapian::percent docScorePercent = it.get_percent();
//            QFileInfo *info = new QFileInfo(QString::fromStdString(data));
            QFileInfo info(QString::fromStdString(data));

            if(!info.exists()) {
//                pathTobeDelete->append(QString::fromStdString(data));
                qDebug() << QString::fromStdString(data) << "is not exist!!";
            } else {
                searchResult.append(QString::fromStdString(data));
            }

            qDebug() << "doc=" << QString::fromStdString(data) << ",weight=" << docScoreWeight << ",percent=" << docScorePercent;
        }
//        //Delete those path doc which is not already exist.
//        if(!pathTobeDelete->isEmpty())
//            deleteAllIndex(pathTobeDelete);

        qDebug() << "--search finish--";
    } catch(const Xapian::Error &e) {
        qDebug() << QString::fromStdString(e.get_description());
    }
    return searchResult;
}

bool IndexGenerator::deleteAllIndex(QStringList *pathlist) {
    QStringList *list = pathlist;
    if(list->isEmpty())
        return true;
    try {
        qDebug() << "--delete start--";
        for(int i = 0; i < list->size(); i++) {
            QString doc = list->at(i);
            std::string uniqueterm = FileUtils::makeDocUterm(doc);
            std::string upterm = "ZEEKERUPTERM" + FileUtils::makeDocUterm(doc);

            m_database_path->delete_document(uniqueterm);
            m_database_content->delete_document(uniqueterm);
            m_database_ocr->delete_document(uniqueterm);

            //delete all files under it if it's a dir.
            m_database_path->delete_document(upterm);
            m_database_content->delete_document(upterm);
            m_database_ocr->delete_document(upterm);

            qDebug() << "delete path" << doc;
//            qDebug() << "delete md5" << QString::fromStdString(uniqueterm);

            //            qDebug()<<"m_database_path->get_lastdocid()!!!"<<m_database_path->get_lastdocid();
            //            qDebug()<<"m_database_path->get_doccount()!!!"<<m_database_path->get_doccount();
        }
        m_database_path->commit();
        m_database_content->commit();
        m_database_ocr->commit();
        qDebug() << "--delete finish--";
    } catch(const Xapian::Error &e) {
        qWarning() << QString::fromStdString(e.get_description());
        return false;
    }

    Q_EMIT this->transactionFinished();
    return true;
}

bool IndexGenerator::deleteContentIndex(QStringList *pathlist)
{
    if(pathlist->isEmpty())
        return true;
    try {
        qDebug() << "--delete start--";
        for(int i = 0; i < pathlist->size(); i++) {
            QString doc = pathlist->at(i);
            std::string uniqueterm = FileUtils::makeDocUterm(doc);
            m_database_content->delete_document(uniqueterm);
            qDebug() << "delete path" << doc;
        }
        m_database_content->commit();
        qDebug() << "--delete finish--";
    } catch(const Xapian::Error &e) {
        qWarning() << QString::fromStdString(e.get_description());
        return false;
    }
    return true;
}

bool IndexGenerator::deleteOcrIndex(QStringList *pathlist)
{
    if(pathlist->isEmpty())
        return true;
    try {
        qDebug() << "--delete start--";
        for(int i = 0; i < pathlist->size(); i++) {
            QString doc = pathlist->at(i);
            std::string uniqueterm = FileUtils::makeDocUterm(doc);
            m_database_ocr->delete_document(uniqueterm);
            qDebug() << "delete path" << doc;
        }
        m_database_ocr->commit();
        qDebug() << "--delete finish--";
    } catch(const Xapian::Error &e) {
        qWarning() << QString::fromStdString(e.get_description());
        return false;
    }
    return true;
}

bool IndexGenerator::updateIndex(QVector<PendingFile> *pendingFiles)
{

    QQueue<QVector<QString>> *fileIndexInfo = new QQueue<QVector<QString>>;
    QQueue<QString> *fileContentIndexInfo = new QQueue<QString>;
    QQueue<QString> *fileOcrIndexInfo = new QQueue<QString>;
    QStringList *deleteList = new  QStringList;
    QStringList *contentDeleteList = new  QStringList;
    for (PendingFile file : *pendingFiles) {
        if (file.shouldRemoveIndex()) {
            deleteList->append(file.path());
            continue;
        }
        fileIndexInfo->append(QVector<QString>() << file.path().section("/" , -1) << file.path() << QString(file.isDir() ? "1" : "0"));
        if ((!file.path().split(".").isEmpty()) && (true == targetFileTypeMap[file.path().section("/" , -1) .split(".").last()])) {
            if (!FileUtils::isEncrypedOrUnreadable(file.path())) {
                fileContentIndexInfo->append(file.path());
            } else {
                contentDeleteList->append(file.path());
            }
        }
    }
    if (!deleteList->isEmpty()) {
        deleteAllIndex(deleteList);
    }
    if (!contentDeleteList->isEmpty()) {
        deleteContentIndex(contentDeleteList);
    }
    if (!fileIndexInfo->isEmpty()) {
        creatAllIndex(fileIndexInfo);
    }
    if (!fileContentIndexInfo->isEmpty()) {
        creatAllIndex(fileContentIndexInfo);
    }
    if (!fileOcrIndexInfo->isEmpty()) {
        creatOcrIndex(fileOcrIndexInfo);
    }
    if (fileIndexInfo) {
        delete fileIndexInfo;
        fileIndexInfo = nullptr;
    }
    if (fileContentIndexInfo) {
        delete fileContentIndexInfo;
        fileContentIndexInfo = nullptr;
    }
    if (fileOcrIndexInfo) {
        delete fileOcrIndexInfo;
        fileOcrIndexInfo = nullptr;
    }
    if (deleteList) {
        delete deleteList;
        deleteList = nullptr;
    }
    if (contentDeleteList) {
        delete contentDeleteList;
        contentDeleteList = nullptr;
    }

    return true;
}

