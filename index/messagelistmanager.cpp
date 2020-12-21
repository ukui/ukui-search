#include "messagelistmanager.h"
#include <QDebug>
MessageListManager::MessageListManager(){
    this->messageList = new QStringList();
    this->ig = IndexGenerator::getInstance();
}

MessageListManager::~MessageListManager(){
    delete this->messageList;
    //delete this->ig;

    this->messageList = nullptr;
    this->ig = nullptr;
}

void MessageListManager::AddMessage(const QString& path){
    this->messageList->append(path);
    if (static_cast<size_t>(this->messageList->length()) >= this->length){
        this->SendMessage();
    }
}

bool MessageListManager::SendMessage(){
    if (this->messageList->empty()){
        return true;
    }

    this->ig->creatAllIndex(this->messageList);
    this->messageList->clear();
    return true;
}

void MessageListManager::SetAutoSendMessageLength(const size_t& length){
    this->length = length;
}
