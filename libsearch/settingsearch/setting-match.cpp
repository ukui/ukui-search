#include "setting-match.h"
#include "file-utils.h"
SettingsMatch::SettingsMatch(QObject *parent) : QObject(parent)
{
    xmlElement();
}

/**
 * @brief SettingsMatch::startMatchApp
 * 返回给页面
 * @param source
 * 获取的字符串
 * @return
 */
QStringList SettingsMatch::startMatchApp(const QString &source){
    m_sourceText=source;
//    qDebug()<<m_sourceText;
    QStringList settingList=matching();
    return settingList;
}

/**
 * @brief SettingsMatch::xmlElement
 * 将xml文件内容读到内存
 */
void SettingsMatch::xmlElement(){
    QString pinyinIndex;
    QString ChineseIndex;

    QFile file(QString::fromLocal8Bit("/usr/share/ukui-control-center/shell/res/search.xml"));
    if (!file.open(QIODevice::ReadOnly)){
        return;
    }
    QDomDocument doc;
    doc.setContent(&file);
    QDomElement root=doc.documentElement();
    QDomNode node = root.previousSibling();
    node=root.firstChild();

    while(!node.isNull()){
        QDomElement element=node.toElement();
        QString key=element.attribute("name");;
        m_chine_searchResult=m_chine_searchList.value(key);
        m_pinyin_searchResult=m_pinyin_searchList.value(key);
         QDomNodeList list=element.childNodes();
         for(int i=0;i<list.count();++i){
             QDomNode n=list.at(i);

             if(n.nodeName()==QString::fromLocal8Bit("pinyinPlugin")){
                 pinyinIndex=n.toElement().text();
             }
             if(n.nodeName()==QString::fromLocal8Bit("pinyinfunc")){
                 pinyinIndex+=QString::fromLocal8Bit("/")+n.toElement().text();
                 m_pinyin_searchResult.append(pinyinIndex);
             }

             if(n.nodeName()==QString::fromLocal8Bit("ChinesePlugin")){
                 ChineseIndex=n.toElement().text();
             }
             if(n.nodeName()==QString::fromLocal8Bit("ChineseFunc")){
                 ChineseIndex+=QString::fromLocal8Bit("/")+n.toElement().text();

             m_chine_searchResult.append(ChineseIndex);
             }
         }
        m_pinyin_searchList.insert(key,m_pinyin_searchResult);
        m_chine_searchList.insert(key,m_chine_searchResult);

        node=node.nextSibling();
    }
    file.close();
//    qDebug()<<pinyin_searchlist;
//    qDebug()<<chine_searchlist;
}

/**
 * @brief SettingsMatch::matching
 * 进行关键字匹配
 * @return
 */
QStringList SettingsMatch::matching(){
    QStringList returnresult;
    QStringList regmatch;
    QString key;
    QMap<QString, QStringList>::const_iterator i;
    for(i=m_chine_searchList.constBegin();i!=m_chine_searchList.constEnd();++i){
        regmatch=*i;
        key=i.key();
//        qDebug()<<key;
        for(int t=0; t<regmatch.size();t++)
        {
            QString str =regmatch.at(t);
            if(str.contains(m_sourceText))
            {
                str=key+"/"+str;
                returnresult.append(str);//中文名
                continue;
            }
             QString pinyin=FileUtils::findMultiToneWords(str).at(0);// 中文转拼音
             if(pinyin.contains(m_sourceText,Qt::CaseInsensitive)){
                 str=key+"/"+str;
                 returnresult.append(str);
                 continue;
             }
             QString shouzimu=FileUtils::findMultiToneWords(str).at(1);// 中文转首字母
             if(shouzimu.contains(m_sourceText,Qt::CaseInsensitive)){
                 str=key+"/"+str;
                 returnresult.append(str);
             }
        }
    }
//    qDebug()<<returnresult;
    return returnresult;
}
