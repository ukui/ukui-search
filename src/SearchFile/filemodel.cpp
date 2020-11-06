#include "filemodel.h"
#include <stdio.h>
#include <pwd.h>
#include <unistd.h>
filemodel::filemodel():
    startmatchTimer(new QTimer(this)),
    cmd(new QProcess(this))
{
    startmatchTimer->setSingleShot(true);
    startmatchTimer->setInterval(10);
    connect(startmatchTimer,&QTimer::timeout,this,[=](){
        matching();
    });

}

int filemodel::rowCount(const QModelIndex& index) const
{
    return index.isValid() ? 0 : returnresult.count();
}


QVariant filemodel::headerData(int section,Qt::Orientation orientation ,int role) const {
    if(role == 0){
        return tr("File");
    }
    return QAbstractItemModel::headerData(section,orientation,role);
}

QVariant filemodel::data(const QModelIndex &index, int role) const
{
    if (role == 0) {
        return returnresult.at(index.row());
    }
    return QVariant();
}

void filemodel::matchstart(const QString &source){

        sourcetext=source;
        returnresult.clear();
        pathresult.clear();
        runresult.clear();
        if(sourcetext.isEmpty()){
            matchesChanged();
            return;
        }
        startmatchTimer->start();
}

void filemodel::matching(){
    sourcetext=QString::fromLocal8Bit("*")+sourcetext+QString::fromLocal8Bit("*");
    commandsearch();
}

void filemodel::matchesChanged()
{
    m_filenum = 0;
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

void filemodel::commandsearch(){

    struct passwd *pwd;
    pwd=getpwuid(getuid());
        QString str =sourcetext;
        QString name =QString::fromLocal8Bit(pwd->pw_name)+QString::fromLocal8Bit("/");
        QString command=QString::fromLocal8Bit("locate /home/")+name+str;
        qDebug()<<command;
        cmd->setReadChannel(QProcess::StandardOutput);
        cmd->start(command);
        cmd->startDetached(cmd->program());
        cmd->waitForFinished();

        connect(cmd,&QProcess::readyReadStandardOutput,this,[=](){
            QString result=QString::fromLocal8Bit(cmd->readAllStandardOutput());
            if(!result.isEmpty()){
                showResult(result);
            }
        });
        cmd->close();
}

void filemodel::showResult(QString result){
    pathresult=result.split(QString::fromLocal8Bit("\n"));
     for(int i=0;i<pathresult.count();i++)
     {
         QList<QString> str1=pathresult.at(i).split(QString::fromLocal8Bit("/"));
         if(str1.at(str1.count()-1).size()<20){
         returnresult.append(str1.at(str1.count()-1));
         runresult.append(pathresult.at(i));
         m_filenum=runresult.count();

         }else{
             return;
         }
     }
    matchesChanged();
}

void filemodel::run(int index){
    QString file=runresult.at(index);
//    qDebug()<<returnresult.at(index);
//    qDebug()<<file;
    QString m_szHelpDoc = QString(QString::fromLocal8Bit("file:///")) + file;
//    bool is_open = QDesktopServices::openUrl(QUrl(m_szHelpDoc, QUrl::TolerantMode));
    QString  cmd= QString(QString::fromLocal8Bit("xdg-open "))+ m_szHelpDoc;
    QProcess app;
    app.setProgram(cmd);
    app.startDetached(app.program());
    app.waitForFinished(-1);

}

int filemodel::listenchanged()
{
   return m_filenum;
}
