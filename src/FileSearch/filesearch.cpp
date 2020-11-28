#include "filesearch.h"
#include <stdio.h>
#include <pwd.h>
#include <unistd.h>

/**
  * @brief filesearch::filesearch 文件搜索的实现类，将输入的内容在用户目录下进行匹配
  * @param searchInput 匹配文件名字的输入
  */
 filesearch::filesearch(QString searchInput)
{
    test=searchInput;
    struct passwd *pwd;
    pwd=getpwuid(getuid());
    QString path="/home/"+QString(pwd->pw_name);
    if(!test.isEmpty())
        this->FindFile(path);
}

filesearch::~filesearch()
{
}

/**
 * @brief filesearch::FindFile 文件递归遍历，在遍历的过程中匹配
 * @param _filePath 文件遍历的路径
 * @return 目前没有用
 */
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

                //这里是获取当前要处理的文件名，原文匹配
                QString sourceText=infolist.at(m);
                if(sourceText.contains(test)){
                    searchResult.insert(sourceText,_filePath);
                }

                //拼音匹配
                QString pinyin=UkuiChineseLetter::getPinyins(sourceText).toLower(); // 中文转英文
                if(pinyin.contains(test,Qt::CaseInsensitive)){
                    searchResult.insert(sourceText,_filePath);
                }

                //首字母匹配
                QString pinyinFrist=UkuiChineseLetter::getFirstLettersAll(sourceText).toLower();// 中文转首字母
                if(pinyinFrist.contains(test,Qt::CaseInsensitive)){
                    searchResult.insert(sourceText,_filePath);
                }
            }
            break;
        }//end else
        i++;
    } while(i < list.size());
    return 1;
}

/**
 * @brief filesearch::returnResult 将遍历匹配得到的结果进行返回
 * @return 匹配文件的路径和名字
 */
QMap<QString,QString> filesearch::returnResult(){
    return searchResult;
}

