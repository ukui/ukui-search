#include "messagelisttemplate.h"
#include <QtConcurrent>

//重构部分，暂时不要调用
template<typename MESSAGETYPE, typename SOMETHINGINSTANCE>
MessageVectorTemplate<MESSAGETYPE, SOMETHINGINSTANCE>::MessageVectorTemplate::MessageVectorTemplate(SOMETHINGINSTANCE* somethingInstance)
{
    this->somethingInstance = somethingInstance;
}

template<typename MESSAGETYPE, typename SOMETHINGINSTANCE>
bool MessageVectorTemplate<MESSAGETYPE, SOMETHINGINSTANCE>::MessageVectorTemplate::addMessage(const MESSAGETYPE& message)
{
    this->messageVec->append(message);
}

template<typename MESSAGETYPE, typename SOMETHINGINSTANCE>
bool MessageVectorTemplate<MESSAGETYPE, SOMETHINGINSTANCE>::sendMessage()
{
    QtConcurrent::run(
                [&](){
//        if (this->m_mutex.try_lock(this->m_time)){
//            std::cout << "send_test_time" << std::endl;
//            this->m_mutex.unlock();
//        }
    });
}

template<typename MESSAGETYPE, typename SOMETHINGINSTANCE>
bool MessageVectorTemplate<MESSAGETYPE, SOMETHINGINSTANCE>::MessageVectorTemplate::sendDeleteMessage()
{

}

template<typename MESSAGETYPE, typename SOMETHINGINSTANCE>
void MessageVectorTemplate<MESSAGETYPE, SOMETHINGINSTANCE>::MessageVectorTemplate::setAutoSendMessageLength(const size_t& length)
{
    this->m_length = length;
}

template<typename MESSAGETYPE, typename SOMETHINGINSTANCE>
void MessageVectorTemplate<MESSAGETYPE, SOMETHINGINSTANCE>::MessageVectorTemplate::setAutoSendMessageTime(const size_t& time)
{
    this->m_time = time;
}
