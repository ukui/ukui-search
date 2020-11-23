#include "filesearch.h"
#include <stdio.h>
#include <pwd.h>
#include <unistd.h>

 filesearch::filesearch(QString searchInput)
{
    test=searchInput;
    struct passwd *pwd;
    pwd=getpwuid(getuid());
    QString path="/home/"+QString(pwd->pw_name);
    this->FindFile(path);
}

filesearch::~filesearch()
{
}


int filesearch::FindFile(const QString& _filePath)
{
    QDir dir(_filePath);
    if (!dir.exists()) {
        return -1;
    }

  //取到所有的文件和文件名，但是去掉.和..的文件夹（这是QT默认有的）
    dir.setFilter(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot);

    //文件夹优先
    dir.setSorting(QDir::DirsFirst);

    //转化成一个list
    QFileInfoList list = dir.entryInfoList();
    QStringList infolist = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);
    if(list.size()< 1 ) {
        return -1;
    }
    int i=0;
    //递归算法的核心部分
    do{
        QFileInfo fileInfo = list.at(i);
        //如果是文件夹，递归
        bool bisDir = fileInfo.isDir();
        if(bisDir) {
            FindFile(fileInfo.filePath());
        }
        else{
            for(int m = 0; m <infolist.size(); m++) {
                                //这里是获取当前要处理的文件名
                QString str=infolist.at(m);
                if(str.contains(test)){
                    searchResult.insert(_filePath,str);
                }
            }
            break;
        }//end else
        i++;
    } while(i < list.size());
    return 1;
}

QMap<QString,QString> filesearch::returnResult(){
    return searchResult;
}

