//#include <QtConcurrent>
#include "first-index.h"
#include <QDebug>

#define NEW_QUEUE(a) a = new QQueue<QString>(); qDebug("---------------------------%s %s %s new at %d..",__FILE__,__FUNCTION__,#a,__LINE__);
//#define DELETE_QUEUE(a )

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
    //this->q_content_index = new QQueue<QString>();
    NEW_QUEUE(this->q_content_index);
//    this->mlm = new MessageListManager();
}

FirstIndex::~FirstIndex()
{
    qDebug() << "~FirstIndex";
    if(this->q_index)
        delete this->q_index;
    this->q_index = nullptr;
    if(this->q_content_index)
        delete this->q_content_index;
    this->q_content_index = nullptr;
    if (this->p_indexGenerator)
        delete this->p_indexGenerator;
    this->p_indexGenerator = nullptr;
    qDebug() << "~FirstIndex end";
//    delete this->mlm;
//    this->mlm = nullptr;
}

void FirstIndex::DoSomething(const QFileInfo& fileInfo){
//    qDebug() << "there are some shit here"<<fileInfo.fileName() << fileInfo.absoluteFilePath() << QString(fileInfo.isDir() ? "1" : "0");
    this->q_index->enqueue(QVector<QString>() << fileInfo.fileName() << fileInfo.absoluteFilePath() << QString(fileInfo.isDir() ? "1" : "0"));
    for (auto i : this->targetFileTypeVec){
        if (fileInfo.fileName().endsWith(i)){
            this->q_content_index->enqueue(fileInfo.absoluteFilePath());
        }
    }
}

void FirstIndex::run(){
    int fifo_fd;
    char buffer[2];
    memset(buffer, 0, sizeof(buffer));
    buffer[0] = 0x1;
    buffer[1] = '\0';
    fifo_fd = open(UKUI_SEARCH_PIPE_PATH, O_RDWR);
    if(fifo_fd == -1)
    {
        perror("open fifo error\n");
        assert(false);
    }

    if (this->bool_dataBaseExist){
        if (this->bool_dataBaseStatusOK){
            int retval = write(fifo_fd, buffer, strlen(buffer));
            if(retval == -1)
            {
                perror("write error\n");
            }
            printf("write data ok!\n");

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
            p_indexGenerator = IndexGenerator::getInstance(true,this);
        }
    }
    else{
        p_indexGenerator = IndexGenerator::getInstance(false,this);
    }

//    this->q_content_index->enqueue(QString("/home/zhangzihao/Desktop/qwerty/四库全书.txt"));

//    this->p_indexGenerator->creatAllIndex(this->q_content_index);


    ++FileUtils::_index_status;

    pid_t pid;
    pid = fork();
    if(pid  == 0)
    {
        prctl(PR_SET_PDEATHSIG, SIGTERM);
        prctl(PR_SET_NAME,"first-index");
        QSemaphore sem(5);
        QMutex mutex1, mutex2, mutex3;
        mutex1.lock();
        mutex2.lock();
        mutex3.lock();
        sem.acquire(4);
        QtConcurrent::run([&](){
            sem.acquire(1);
            mutex1.unlock();
            this->setPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
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
        ::exit(0);




        //    qDebug() << "first index end;";
        //don't use it now!!!!
        //MouseZhangZh
        //    this->~FirstIndex();
        //    qDebug() << "~FirstIndex end;"


        if (this->q_index)
            delete this->q_index;
        this->q_index = nullptr;
        if (this->q_content_index)
            delete this->q_content_index;
        this->q_content_index = nullptr;
        if (p_indexGenerator)
            delete p_indexGenerator;
        p_indexGenerator = nullptr;

        QThreadPool::globalInstance()->releaseThread();
        QThreadPool::globalInstance()->waitForDone();
    }
    else if(pid < 0)
    {
        qWarning()<<"First Index fork error!!";
    }
    else
    {
        waitpid(pid,NULL,0);
        --FileUtils::_index_status;
    }

    int retval = write(fifo_fd, buffer, strlen(buffer));
    if(retval == -1)
    {
        perror("write error\n");
    }
    printf("write data ok!\n");


    //quit() is shit!!!
//    return;
//    exit(0);
    this->quit();
//    this->wait();
}
