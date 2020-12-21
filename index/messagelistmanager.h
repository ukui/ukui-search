#ifndef MESSAGELISTMANAGER_H
#define MESSAGELISTMANAGER_H

#include <QObject>
#include "index-generator.h"

class MessageListManager : public QObject
{
    Q_OBJECT
public:
    explicit MessageListManager();
    ~MessageListManager();

    void AddMessage(const QString&);
    bool SendMessage();
    void SetAutoSendMessageLength(const size_t&);

private:
    QStringList* messageList;
    size_t length = 0;
    IndexGenerator* ig;
Q_SIGNALS:

};

#endif // MESSAGELISTMANAGER_H
