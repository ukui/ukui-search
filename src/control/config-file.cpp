#include "config-file.h"

ConfigFile::ConfigFile(QObject *parent) : QObject(parent)
{
    QFile file(QDir::homePath()+"/.config/org.ukui/ukui-search/ukui-search.conf");
    if(!file.exists()){
        file.open( QIODevice::ReadWrite | QIODevice::Text );
        file.close();
    }
    m_qSettings=new QSettings(QDir::homePath()+"/.config/org.ukui/ukui-search/ukui-search.conf",QSettings::IniFormat);
    receiveMessage("ukui-panel");//测试使用
    receiveMessage("ukui-panel.desktop");//测试使用
    readConfig();//页面调用
}

void ConfigFile::writeCommonly(QString message){
    QStringList messagelist=message.split("/");
    QString appname=messagelist.last();
    if(!appname.contains("desktop"))
        return;
    m_qSettings->beginGroup("Commonly");
    QStringList quickly=m_qSettings->allKeys();
    if(quickly.contains(message)){
        m_qSettings->setValue(message,m_qSettings->value(message).toInt()+1);
    }else{
        m_qSettings->setValue(message,1);
    }
    m_qSettings->endGroup();
}

QStringList ConfigFile::readCommonly(){
    QStringList returnlist;
    QMap<QString,int> quicklycount;
    m_qSettings->beginGroup("Commonly");
    QStringList Commonly=m_qSettings->allKeys();
    for(int i=0;i<Commonly.size();i++){
        quicklycount.insert(Commonly.at(i),m_qSettings->value(Commonly.at(i)).toInt());
    }
    m_qSettings->endGroup();
    QMap<QString, int>::iterator iter =quicklycount.begin();
    QVector<QPair<QString, int>> vec;
    QString iconamePah;
    while(iter !=quicklycount.end()) {
        vec.push_back(qMakePair(iter.key(), iter.value()));
        iter++;
    }
    qSort(vec.begin(), vec.end(), [](const QPair<QString, int> &l, const QPair<QString, int> &r) {
        return (l.second > r.second);
    });
    for(int j=0;j<vec.size();j++){
        returnlist.append(vec.at(j).first);
    }
//    qDebug()<<returnlist;
    return returnlist;
}

void ConfigFile::writeRecently(QString message){
    m_qSettings->beginGroup("Recently");
    QStringList recently=m_qSettings->value("Recently").toStringList();
    m_qSettings->endGroup();
    recently.insert(0,message);
    m_qSettings->beginGroup("Recently");
    m_qSettings->setValue("Recently",recently);
    m_qSettings->endGroup();
}

QStringList ConfigFile::readRecently(){
    m_qSettings->beginGroup("Recently");
    QStringList recently=m_qSettings->value("Recently").toStringList();
    m_qSettings->endGroup();
//    qDebug()<<recently;
    return recently;
}

void ConfigFile::writeConfig(){
    writeCommonly(m_message);
    writeRecently(m_message);
}

QMap<QString,QStringList> ConfigFile::readConfig(){
    QMap<QString,QStringList> returnresult;
    returnresult.insert("Commonly",readCommonly());
    returnresult.insert("Recently",readRecently());
//    qDebug()<<returnresult;
    return returnresult;
}

void ConfigFile::receiveMessage(QString message){
    m_message=message;
    writeConfig();
}
