#ifndef FIRSTINDEX_H
#define FIRSTINDEX_H

#include <QThread>
#include <QtConcurrent/QtConcurrent>
//#include <QtConcurrent>
#include "traverse_bfs.h"
#include "global-settings.h"
#include "index-generator.h"
#include "messagelist-manager.h"

class FirstIndex : public QThread, public Traverse_BFS
{
public:
    FirstIndex(const QString&);
    ~FirstIndex();
    virtual void DoSomething(const QFileInfo &) final;
protected:
    void run() override;
private:
    FirstIndex(const FirstIndex&) = delete;
    void operator=(const FirstIndex&) = delete;
    bool bool_dataBaseStatusOK = false;
    bool bool_dataBaseExist = false;
    IndexGenerator* p_indexGenerator;

    //here should be refact
//    MessageListManager* mlm;

    //test
    QQueue<QVector<QString>>* q_index;
    QQueue<QString>* q_content_index;
    const QVector<QString> targetFileTypeVec ={
//        QString(".doc"),
        QString(".docx"),
//        QString(".ppt"),
//        QString(".pptx"),
//        QString(".xls"),
//        QString(".xlsx"),
        QString(".txt")};
};

#endif // FIRSTINDEX_H
