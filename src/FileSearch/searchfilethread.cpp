#include "searchfilethread.h"

SearchFileThread::SearchFileThread()
{
}

void SearchFileThread::run(){

    filesearch *Fsearch= new filesearch(m_keyWord);
    recMap=Fsearch->returnResult();
    QMap<QString, QString>::const_iterator i;
            for(i=recMap.constBegin();i!=recMap.constEnd();++i){
               QString str=i.key()+"/"+i.value();
               returnResult.append(str);
            }
            Q_EMIT sendSearchResult(returnResult);
}

void SearchFileThread::slotFunc(QString arg)
{
    mSearchline = arg;
}

SearchFileThread::~SearchFileThread(){

}

void SearchFileThread::recvSearchKeyword(QString arg)
{
    this->returnResult.clear();
    this->m_keyWord.clear();
    this->m_keyWord=arg;
}
