#include "config-file.h"

bool ConfigFile::writeCommonly(QString message){
    QSettings *m_qSettings=new QSettings(QDir::homePath()+"/.config/org.ukui/ukui-search/ukui-search.conf",QSettings::IniFormat);
    QStringList messagelist=message.split("/");
    QString appname=messagelist.last();
    if(!appname.contains("desktop"))
        return false;
    m_qSettings->beginGroup("Commonly");
    QStringList quickly=m_qSettings->allKeys();
    if(quickly.contains(message.mid(1, message.length()-1))){
        m_qSettings->setValue(message,m_qSettings->value(message).toInt()+1);
    }else{
        m_qSettings->setValue(message,1);
    }
    m_qSettings->endGroup();
    if(m_qSettings)
        delete m_qSettings;
    return true;
}

QStringList ConfigFile::readCommonly(){
    QSettings *m_qSettings=new QSettings(QDir::homePath()+"/.config/org.ukui/ukui-search/ukui-search.conf",QSettings::IniFormat);
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
    while(iter !=quicklycount.end()) {
        vec.push_back(qMakePair(iter.key(), iter.value()));
        iter++;
    }
    qSort(vec.begin(), vec.end(), [](const QPair<QString, int> &l, const QPair<QString, int> &r) {
        return (l.second > r.second);
    });
    for(int j=0;j<vec.size();j++){
        returnlist.append("/" + vec.at(j).first);
    }
    if(m_qSettings)
        delete m_qSettings;
    return returnlist.mid(0, 4);
}

bool ConfigFile::writeRecently(QString message){
    QSettings *m_qSettings=new QSettings(QDir::homePath()+"/.config/org.ukui/ukui-search/ukui-search.conf",QSettings::IniFormat);

    m_qSettings->beginGroup("Recently");
    QStringList recently=m_qSettings->value("Recently").toStringList();
    m_qSettings->endGroup();
    if(recently.contains(message)){
        recently.removeOne(message);
    }
    recently.insert(0,message);

    m_qSettings->beginGroup("Recently");
    if (m_qSettings->value("Recently").toStringList().length() >= 20) {
        m_qSettings->setValue("Recently",QStringList(recently.mid(0, 20)));
    } else {
        m_qSettings->setValue("Recently",recently);
    }
    m_qSettings->endGroup();
    if(m_qSettings)
        delete m_qSettings;
    return true;
}

QStringList ConfigFile::readRecently(){
    QSettings *m_qSettings=new QSettings(QDir::homePath()+"/.config/org.ukui/ukui-search/ukui-search.conf",QSettings::IniFormat);

    m_qSettings->beginGroup("Recently");
    QStringList recently=m_qSettings->value("Recently").toStringList();
    m_qSettings->endGroup();
    if(m_qSettings)
        delete m_qSettings;
    return recently.mid(0, 4);
}

bool ConfigFile::writeConfig(QString message){
    bool isWriteCommonlyDone = writeCommonly(message);
    bool isWriteRecentlyDone = writeRecently(message);
    return (isWriteCommonlyDone || isWriteRecentlyDone);
}

QMap<QString,QStringList> ConfigFile::readConfig(){
    QMap<QString,QStringList> returnresult;
    returnresult.insert("Commonly",readCommonly());
    returnresult.insert("Recently",readRecently());
    return returnresult;
}

void ConfigFile::receiveMessage(QString message){
    QFile file(QDir::homePath()+"/.config/org.ukui/ukui-search/ukui-search.conf");
    if(!file.exists()){
        file.open( QIODevice::ReadWrite | QIODevice::Text );
        file.close();
    }
    readConfig();//页面调用
    writeConfig(message);
}
