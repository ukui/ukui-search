#include "settingmodel.h"


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
    return index.isValid() ? 0 : returnresult.count();
}

//重写设置搜索的model标头
QVariant settingModel::headerData(int section,Qt::Orientation orientation ,int role) const {
    if(role == 0){
        return tr("系统设置");
    }

    return QAbstractItemModel::headerData(section,orientation,role);
}

QVariant settingModel::data(const QModelIndex &index, int role) const
{
    QPixmap map(QString::fromLocal8Bit("/home/li/ukui/ukui-search1/ukui-search/data/img/mainviewwidget/search.png"));

    if (role == Qt::DisplayRole) {
        return returnresult.at(index.row());
    }else if(role == Qt::DecorationRole){
        return iconresult.at(index.row());
    }

    switch(role){
    case Qt::TextColorRole:
//           return QColor(Qt::black);
    case Qt::FontRole:
            if(index.column()==0){
            return QFont("宋体",11,QFont::Bold);
            }
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
}

//按字段解析xml文件，将设置插件的中文提取出来
void settingModel::XmlElement(){

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
                 index1=n.toElement().text();
             }
             if(n.nodeName()==QString::fromLocal8Bit("pinyinfunc")){
                 index1+=QString::fromLocal8Bit(":")+n.toElement().text();
                 pinyin_searchresult.append(index1);
             }

             if(n.nodeName()==QString::fromLocal8Bit("ChinesePlugin")){
                 index=n.toElement().text();
             }
             if(n.nodeName()==QString::fromLocal8Bit("ChineseFunc")){
                 index+=QString::fromLocal8Bit(":")+n.toElement().text();

             chine_searchresult.append(index);
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
        returnresult.clear();
        commandresult.clear();
        Q_EMIT requestUpdateSignal(commandresult.count());
        iconresult.clear();
        if(sourcetext.isEmpty())
        {
             matchesChanged();
            return ;
        }
        startmatchTimer->start();
}

//将编辑框的字符串与xml文件解析出的结果正则表达式匹配
void settingModel::matching(){
    sourcetext+=QString::fromLocal8Bit(".*");
    QRegExp rx(sourcetext);
    QMap<QString, QStringList>::const_iterator i;
    for(i=chine_searchlist.constBegin();i!=chine_searchlist.constEnd();++i){
        regmatch=*i;
        settingkey=i.key();
        QList<QString>::Iterator it = regmatch.begin(),itend = regmatch.end();
        int n = 0;
        for (;it != itend; it++,n++){
            if(rx.exactMatch(*it)){
                returnresult.append(*it);//中文名
                commandresult.append(settingkey);//命令
                QString str="/usr/share/ukui-control-center/shell/res/secondaryleftmenu/"+settingkey+".svg";
                QIcon icon = QIcon::fromTheme(str);
                QPixmap pixmap = icon.pixmap(QSize(25,25));
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
            if(rx.exactMatch(*it)){

                QStringList val=chine_searchlist.value(settingkey);

                returnresult.append(val.at(n));//pinyin
                commandresult.append(settingkey);//命令

                QString str="/usr/share/ukui-control-center/shell/res/secondaryleftmenu/"+settingkey+".svg";
                QIcon icon = QIcon::fromTheme(str);
                QPixmap pixmap = icon.pixmap(QSize(25,25));
                iconresult.append(pixmap);

            }
        }
    }

    Q_EMIT requestUpdateSignal(commandresult.count());
    matchesChanged();
}

//编辑栏内容改变，将model重新刷新
void settingModel::matchesChanged()
{

    bool fullReset = false;
    int newCount = returnresult.count();
    int oldCount = lockresult.count();
    if (newCount > oldCount) {
        for (int row = 0; row < oldCount; ++row) {
            if (!(returnresult.at(row) == lockresult.at(row))) {
                fullReset = true;
                break;
            }
        }
        if (!fullReset) {
            beginInsertRows(QModelIndex(), oldCount, newCount-1);
            endInsertRows();
        }
    } else {
        fullReset = true;
    }
    if (fullReset) {
        beginResetModel();
        endResetModel();
    }
    lockresult=returnresult;

}

