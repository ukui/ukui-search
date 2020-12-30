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

    void AddMessage(const QVector<QString>&);
    bool SendMessage();
    bool SendDeleteMessage();
    void SetAutoSendMessageLength(const size_t&);

private:
//    QStringList* messageList;
    QList<QVector<QString>>* messageList;

    size_t length = 80000;
    IndexGenerator* ig;
    QThread* indexGeneratorThread;

Q_SIGNALS:
    bool Send(QStringList*);
};

#endif // MESSAGELISTMANAGER_H
