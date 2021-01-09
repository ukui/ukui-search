//#include <QtConcurrent>
#include "first-index.h"
#include <QDebug>

FirstIndex::FirstIndex(const QString& path) : Traverse_BFS(path)
{
    QString indexDataBaseStatus =  GlobalSettings::getInstance()->getValue(INDEX_DATABASE_STATE).toString();
    QString contentIndexDataBaseStatus = GlobalSettings::getInstance()->getValue(CONTENT_INDEX_DATABASE_STATE).toString();

    qDebug() << "indexDataBaseStatus: " << indexDataBaseStatus;
    qDebug() << "contentIndexDataBaseStatus: " << contentIndexDataBaseStatus;


    if (indexDataBaseStatus == "" || contentIndexDataBaseStatus == ""){
        this->bool_dataBaseExist = false;
    }
    if (indexDataBaseStatus != "2" || contentIndexDataBaseStatus != "2"){
        this->bool_dataBaseStatusOK = false;
    }

    this->q_index = new QQueue<QVector<QString>>();
    this->q_content_index = new QQueue<QString>();

//    this->mlm = new MessageListManager();
}

FirstIndex::~FirstIndex()
{
    delete this->q_index;
    this->q_index = nullptr;
    this->q_content_index = nullptr;
    delete this->q_content_index;
//    delete this->mlm;
//    this->mlm = nullptr;
}

void FirstIndex::DoSomething(const QFileInfo& fileInfo){
//    qDebug() << "there are some shit here";
    this->q_index->enqueue(QVector<QString>() << fileInfo.fileName() << fileInfo.absoluteFilePath() << QString(fileInfo.isDir() ? "1" : "0"));
    for (auto i : this->targetFileTypeVec){
        if (fileInfo.fileName().endsWith(i)){
            this->q_content_index->enqueue(fileInfo.absoluteFilePath());
        }
    }
}

void FirstIndex::run(){
    if (this->bool_dataBaseExist){
        if (this->bool_dataBaseStatusOK){
            this->quit();
//            this->wait();
        }
        else{
            //if the parameter is false, index won't be rebuild
            //if it is true, index will be rebuild
            this->p_indexGenerator = IndexGenerator::getInstance(true);
        }
    }
    else{
        this->p_indexGenerator = IndexGenerator::getInstance(false);
    }
    QSemaphore sem(5);
    QMutex mutex1, mutex2, mutex3;
    mutex1.lock();
    mutex2.lock();
    mutex3.lock();
    sem.acquire(4);
    QtConcurrent::run([&](){
        sem.acquire(1);
        mutex1.unlock();
        this->Traverse();
        sem.release(5);
    });
    QtConcurrent::run([&](){
        sem.acquire(2);
        mutex2.unlock();
        qDebug() << "index start;";
        this->p_indexGenerator->creatAllIndex(this->q_index);
        qDebug() << "index end;";
        sem.release(2);
    });
    QtConcurrent::run([&](){
        sem.acquire(2);
        mutex3.unlock();
        qDebug() << "content index start;";
        this->p_indexGenerator->creatAllIndex(this->q_content_index);
        qDebug() << "content index end;";
        sem.release(2);
    });
    mutex1.lock();
    mutex2.lock();
    mutex3.lock();
    sem.acquire(5);
    mutex1.unlock();
    mutex2.unlock();
    mutex3.unlock();
    qDebug() << "first index end;";
    this->quit();
//    this->wait();
}
