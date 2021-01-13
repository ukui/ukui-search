//#include <QtConcurrent>
#include "first-index.h"
#include <QDebug>


void handler(int){
    qDebug() << "Recieved SIGTERM!";
    GlobalSettings::getInstance()->setValue(INDEX_DATABASE_STATE, "2");
    GlobalSettings::getInstance()->setValue(CONTENT_INDEX_DATABASE_STATE, "2");
    GlobalSettings::getInstance()->setValue(INDEX_GENERATOR_NORMAL_EXIT, "2");
    GlobalSettings::getInstance()->setValue(INOTIFY_NORMAL_EXIT, "2");


    qDebug() << "indexDataBaseStatus: " << GlobalSettings::getInstance()->getValue(INDEX_DATABASE_STATE).toString();
    qDebug() << "contentIndexDataBaseStatus: " << GlobalSettings::getInstance()->getValue(CONTENT_INDEX_DATABASE_STATE).toString();

//    InotifyIndex::getInstance("/home")->~InotifyIndex();
    qDebug() << "~IndexGenerator() end!" << endl;

    //wait linux kill this thread forcedly
//    while (true);
}


FirstIndex::FirstIndex(const QString& path) : Traverse_BFS(path)
{
    QString indexDataBaseStatus =  GlobalSettings::getInstance()->getValue(INDEX_DATABASE_STATE).toString();
    QString contentIndexDataBaseStatus = GlobalSettings::getInstance()->getValue(CONTENT_INDEX_DATABASE_STATE).toString();
    QString inotifyIndexStatus = GlobalSettings::getInstance()->getValue(INOTIFY_NORMAL_EXIT).toString();

    qDebug() << "indexDataBaseStatus: " << indexDataBaseStatus;
    qDebug() << "contentIndexDataBaseStatus: " << contentIndexDataBaseStatus;
    qDebug() << "inotifyIndexStatus: " << inotifyIndexStatus;


    if (indexDataBaseStatus == "" || contentIndexDataBaseStatus == ""){
        this->bool_dataBaseExist = false;
    }
    else{
        this->bool_dataBaseExist = true;
    }
    if (indexDataBaseStatus != "2" || contentIndexDataBaseStatus != "2" || inotifyIndexStatus != "2"){
        this->bool_dataBaseStatusOK = false;
    }
    else{
        this->bool_dataBaseStatusOK = true;
    }

    this->q_index = new QQueue<QVector<QString>>();
    this->q_content_index = new QQueue<QString>();

//    this->mlm = new MessageListManager();
}

FirstIndex::~FirstIndex()
{
    qDebug() << "~FirstIndex";
    delete this->q_index;
    this->q_index = nullptr;
    delete this->q_content_index;
    this->q_content_index = nullptr;
//    delete this->p_indexGenerator;
//    this->p_indexGenerator;
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

            //why???????????????????????????????????????????????????????????????
            //why not quit?
//            this->quit();
//            exit(0);
            return;
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
        FileUtils::_max_index_count = this->q_index->length();
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
    //don't use it now!!!!
    //MouseZhangZh
//    this->~FirstIndex();
    qDebug() << "~FirstIndex end;";



    qDebug() << "sigset start!";
    sigset( SIGTERM, handler);
    qDebug() << "sigset end!";

    this->quit();
//    this->wait();
}
