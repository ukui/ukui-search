#include "searchfilethread.h"

/**
 * @brief SearchFileThread::SearchFileThread 文件搜索的单独线程类
 */
SearchFileThread::SearchFileThread()
{

}

/**
 * @brief SearchFileThread::run QThread的虚函数，通过重写run函数来实现单独开辟线程实现文件搜索功能，
 * 其他类通过监听这个线程类的void sendSearchResult(QStringList arg)信号将搜索文件结果通过QStringList进行
 * 传递。
 */
void SearchFileThread::run(){
    filesearch *Fsearch= new filesearch(m_keyWord);
    recMap=Fsearch->returnResult();
    QMap<QString, QStringList>::const_iterator i;
    QStringList keylist;
    QStringList returnlist;
    QStringList pathlist;
    QString fullpath;
    QString name;
    QString path;

    for(i=recMap.constBegin();i!=recMap.constEnd();++i){
        keylist.append(i.key());
    }
    qSort(keylist.begin(),keylist.end(), [](const QString& s1, const QString& s2){
        return s1.size() < s2.size(); });

    for(int m=0;m<10;m++){
        QString str=keylist.at(m);
        returnlist.append(keylist.at(m));
    }
    for(int m=0;m<returnlist.size();m++){
        for(int j=0;j<recMap.value(returnlist.at(m)).size();++j){
            name=returnlist.at(m);
            pathlist=recMap.value(name);
            path=pathlist.at(j);
            fullpath=path+"/"+name;
        }
        returnResult.append(fullpath);
    }
    Q_EMIT sendSearchResult(returnResult);
}

SearchFileThread::~SearchFileThread(){

}

/**
 * @brief SearchFileThread::recvSearchKeyword 用来接收搜索编辑栏的内容，通过m_keyWord传递给线程run函数
 * @param arg 编辑栏输入值
 */
void SearchFileThread::recvSearchKeyword(QString arg)
{
    this->returnResult.clear();
    this->m_keyWord.clear();
    this->m_keyWord=arg;
}
