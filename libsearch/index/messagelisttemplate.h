#ifndef MESSAGELISTTEMPLATE_H
#define MESSAGELISTTEMPLATE_H
#include <QObject>
#include <memory>
#include <mutex>
#include <iostream>
template<typename MESSAGETYPE, typename SOMETHINGINSTANCE>
class MessageVectorTemplate
{
public:
    MessageVectorTemplate(SOMETHINGINSTANCE*);

    bool addMessage(const MESSAGETYPE&);
    bool sendMessage();
    bool sendDeleteMessage();
    void setAutoSendMessageLength(const size_t&);
    void setAutoSendMessageTime(const size_t&);


private:
    std::unique_ptr<std::vector<MESSAGETYPE>> messageVec = std::make_shared<std::vector<MESSAGETYPE>>();
    std::unique_ptr<SOMETHINGINSTANCE> somethingInstance;

    size_t m_length = 80000;
    size_t m_time = 1000;
    std::mutex m_mutex;
};

#endif // MESSAGELISTTEMPLATE_H
