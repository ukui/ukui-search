#include "settingmodel.h"
#include <src/Interface/ukuichineseletter.h>

#define XML_Source QString::fromLocal8Bit("ChineseFunc")
#define XML_Title  QString::fromLocal8Bit("ChinesePlugin")

settingModel::settingModel():
    startmatchTimer(new QTimer(this))
{


    startmatchTimer->setSingleShot(true);
    startmatchTimer->setInterval(10);
    setting = new QSettings(QString::fromLocal8Bit(":/src/ControlCenterSettingsSearch/assets/search.conf"),QSettings::IniFormat);
    keylist<<setting->allKeys();
    for(int i = 0; i< keylist.size();++i)
    {
        settinglist.append(setting->value(keylist.at(i)).toString());
    }
    connect(startmatchTimer,&QTimer::timeout,this,[=](){
        matching();
    });


    XmlElement();
}

int settingModel::rowCount(const QModelIndex& index) const
{
    return index.isValid() ? 0 : m_Showresult.count();
}

QVariant settingModel::data(const QModelIndex &index, int role) const
{
    QPixmap map(QString::fromLocal8Bit("/home/li/ukui/ukui-search1/ukui-search/data/img/mainviewwidget/search.png"));

    switch(role){
    case Qt::DisplayRole:
           return m_Showresult.at(index.row());
    case Qt::DecorationRole:
        if(index.row()<3)
            return m_Iconresult.at(index.row());
    case Qt::SizeHintRole:
        return QSize(200,46);
    case Qt::FontRole:
         return QFont("宋体",14,QFont::DemiBold);

        }
    return QVariant();
}

//运行对应条目的设置
void settingModel::run(int index)
{
    setting->beginGroup(QString::fromLocal8Bit("control-center"));
    QProcess p;
    p.setProgram(setting->value(commandresult.at(index)).toString());
    p.startDetached(p.program());
    p.waitForFinished(-1);
    setting->endGroup();
    if(index==3){
        QProcess *process =new QProcess(this);
        process->startDetached("ukui-control-center");
    }
}

//按字段解析xml文件，将设置插件的中文提取出来
void settingModel::XmlElement(){

    QString indexpinyin;
    QString indexchinese;
    QFile file(QString::fromLocal8Bit(":/src/ControlCenterSettingsSearch/assets/search.xml"));
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
        chine_searchresult=chine_searchlist.value(key);
        pinyin_searchresult=pinyin_searchlist.value(key);
         QDomNodeList list=element.childNodes();
         for(int i=0;i<list.count();++i){
             QDomNode n=list.at(i);

             if(n.nodeName()==QString::fromLocal8Bit("pinyinPlugin")){
                 indexpinyin=n.toElement().text();
             }
             if(n.nodeName()==QString::fromLocal8Bit("pinyinfunc")){
                 indexpinyin+=QString::fromLocal8Bit(":")+n.toElement().text();
                 pinyin_searchresult.append(indexpinyin);
             }

             if(n.nodeName()==QString::fromLocal8Bit("ChinesePlugin")){
                 indexchinese=n.toElement().text();
             }
             if(n.nodeName()==QString::fromLocal8Bit("ChineseFunc")){
                 indexchinese+=QString::fromLocal8Bit(":")+n.toElement().text();
             chine_searchresult.append(indexchinese);
             }
         }
        pinyin_searchlist.insert(key,pinyin_searchresult);
        chine_searchlist.insert(key,chine_searchresult);

        node=node.nextSibling();
    }
    file.close();
}

//匹配初始化
void settingModel::matchstart(const QString &source){

        sourcetext=source;
        m_Showresult.clear();
        m_Iconresult.clear();
        commandresult.clear();
        Q_EMIT requestUpdateSignal(commandresult.count());
        if(sourcetext.isEmpty())
        {
             matchesChanged();
            return ;
        }
        startmatchTimer->start();
}

//将编辑框的字符串与xml文件解析出的结果正则表达式匹配
void settingModel::matching(){
    QList<QString> returnresult;
    QList<QPixmap> iconresult;
    QStringList regmatch;
    QString settingkey;
    QString str;
    QString pinyinFrist;
    QIcon icon;
    QPixmap pixmap;
    QMap<QString, QStringList>::const_iterator i;
    for(i=chine_searchlist.constBegin();i!=chine_searchlist.constEnd();++i){
        regmatch=*i;
        settingkey=i.key();
        QList<QString>::Iterator it = regmatch.begin(),itend = regmatch.end();
        int n = 0;
        for (;it != itend; it++,n++){
            str =regmatch.at(n);
            pinyinFrist=UkuiChineseLetter::getFirstLettersAll(str.mid(0,4)).toLower();
            if(pinyinFrist.contains(sourcetext)){
                returnresult.append(*it);//中文名
                commandresult.append(settingkey);//命令
                str="/usr/share/ukui-control-center/shell/res/secondaryleftmenu/"+settingkey+".svg";
                icon = QIcon::fromTheme(str);
                pixmap = icon.pixmap(QSize(25,25));
                iconresult.append(pixmap);
                continue;
            }
            if(str.contains(sourcetext)){
                returnresult.append(*it);//中文名
                commandresult.append(settingkey);//命令
                str="/usr/share/ukui-control-center/shell/res/secondaryleftmenu/"+settingkey+".svg";
                icon = QIcon::fromTheme(str);
                pixmap = icon.pixmap(QSize(25,25));
                iconresult.append(pixmap);
            }
        }
    }

    for(i=pinyin_searchlist.constBegin();i!=pinyin_searchlist.constEnd();++i){
        regmatch=*i;
        settingkey=i.key();
        QList<QString>::Iterator it = regmatch.begin(),itend = regmatch.end();
        int n = 0;
        for (;it != itend; it++,n++){
            str =regmatch.at(n);
            if(str.contains(sourcetext)){
                QStringList val=chine_searchlist.value(settingkey);
                returnresult.append(val.at(n));//pinyin
                commandresult.append(settingkey);//命令
                str="/usr/share/ukui-control-center/shell/res/secondaryleftmenu/"+settingkey+".svg";
                icon = QIcon::fromTheme(str);
                pixmap = icon.pixmap(QSize(25,25));
                iconresult.append(pixmap);

            }
        }
    }

    m_Showresult=returnresult.mid(0,3);
    m_Iconresult=iconresult.mid(0,3);

    Q_EMIT requestUpdateSignal(m_Showresult.count());
    matchesChanged();
}

//编辑栏内容改变，将model重新刷新
void settingModel::matchesChanged()
{
        beginResetModel();
        endResetModel();
}

