#include "messagelist-manager.h"
#include <QDebug>
#include <QThread>
#include <QStringList>
#include <QTimer>
//#include <unistd.h>

MessageListManager::MessageListManager(){
    this->messageList = new QStringList();
    this->ig = IndexGenerator::getInstance();
//    indexGeneratorThread = new QThread();
//    this->ig->moveToThread(indexGeneratorThread);
//    connect(this,&MessageListManager::Send, this->ig, &IndexGenerator::creatAllIndex/*, Qt::QueuedConnection*/);
//    connect(this,&MessageListManager::Send1, this->ig, [=](QStringList *l){
//        qDebug()<<"send"<<*l;
//    });

}

MessageListManager::~MessageListManager(){
    delete this->messageList;
    delete this->indexGeneratorThread;
    //delete this->ig;

    this->messageList = nullptr;
    this->ig = nullptr;
    this->indexGeneratorThread = nullptr;
}

void MessageListManager::AddMessage(const QVector<QString>& pathVec){
    this->messageList->append(pathVec);
    if (static_cast<size_t>(this->messageList->length()) >= this->length){
        this->SendMessage();
    }
}

bool MessageListManager::SendMessage(){
    //Q_EMIT Send(this->messageList);
    if (this->messageList->empty()){
        return true;
    }

//    Q_EMIT Send(this->messageList);
//    qDebug() << "emit";
    this->ig->creatAllIndex(this->messageList);

    //sleep(1);
    this->messageList->clear();
    return true;
}

bool MessageListManager::SendDeleteMessage(){
    if (this->messageList->empty()){
        return true;
    }

    this->ig->deleteAllIndex(this->messageList);
    this->messageList->clear();
    return true;
}

void MessageListManager::SetAutoSendMessageLength(const size_t& length){
    this->length = length;
}
