#include "filemodel.h"
#include <stdio.h>
#include <pwd.h>
#include <unistd.h>
filemodel::filemodel():
    cmd(new QProcess(this)),
    fileutils(new FileUtils)
{

    header<<tr("File")<<tr("")<<tr("");

}

int filemodel::rowCount(const QModelIndex& index) const
{
    return index.isValid() ? 0 : showresult.count();
}

int filemodel::columnCount(const QModelIndex& index) const
{
    return index.isValid() ? 0 :3;
}

QVariant filemodel::headerData(int section,Qt::Orientation orientation ,int role) const {
    if(role == Qt::DisplayRole&&orientation==Qt::Horizontal){
        return header[section];
    }
    return QAbstractItemModel::headerData(section,orientation,role);
}

QVariant filemodel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return filemodel::data(index,role);
    switch(role){
    case Qt::DisplayRole:
        if(index.row()<3){
            switch(index.column()){
            case 0:
                return m_showMap.value(QString::fromLocal8Bit("FileName")).at(index.row());
            case 1:
                return m_showMap.value(QString::fromLocal8Bit("PathName")).at(index.row());
            case 2:
                return m_showMap.value(QString::fromLocal8Bit("FileTime")).at(index.row());
            }
        }else {
            if(index.column()==0){
                return showresult.at(index.row());
            }else{
                break;
            }
        }
    case Qt::TextAlignmentRole:
        if(index.column()==0){
            return Qt::AlignLeft;
        }else{
            return  Qt::AlignRight;
        }
    case Qt::ToolTipRole:
        if(index.row()<3){
        return runresult.at(index.row());
        }else {
         return QString::fromLocal8Bit("使用文件管理器搜索");
        }
    case Qt::DecorationRole:
        if(index.row()<3){
            if(index.column()==0){
                return iconresult.at(index.row());
            }else{
                break;
            }
        }else{
            break;
        }
    case Qt::FontRole:
        if(index.column()==0){
            return QFont("宋体",12,QFont::Bold);
        }else{
            return QFont("黑体",8,QFont::Bold);
        }
    case Qt::SizeHintRole:
        return QSize(200,40);
    case Qt::TextColorRole:
        if(index.column()==0){
            return QColor(Qt::blue);
        }
    }
return QVariant();
}

void filemodel::matchstart(const QString &source){

        returnresult.clear();
        pathresult.clear();
        runresult.clear();
        Q_EMIT requestUpdateSignal(runresult.count());
        iconresult.clear();
        openPathresult.clear();
        showresult.clear();
        sourcetext=source;
        peonyText=source;
        matchesChanged();
        if(sourcetext.isEmpty()){
            matchesChanged();
            return;
        }
}

void filemodel::matchesChanged()
{

        beginResetModel();
        endResetModel();
}

void filemodel::showResult(QStringList result){
    pathresult=result;
     for(int i=0;i<pathresult.count();i++)
     {
         QList<QString> str1=pathresult.at(i).split(QString::fromLocal8Bit("/"));
         QString str2;
         str2+=str1.at(0);
         for(int j=1;j<str1.count()-1;j++){
             str2 +=QString::fromLocal8Bit("/")+str1.at(j);
         }
         if(str1.at(str1.count()-1).size()<40){
             returnresult.append(str1.at(str1.count()-1));
             runresult.append(pathresult.at(i));
             openPathresult.append(str2);

             QString str= QString::fromLocal8Bit("file://")+pathresult.at(i);
             QIcon icon= QIcon::fromTheme(fileutils->getFileIconName(str));
             QPixmap pixmap = icon.pixmap(QSize(40,40));
             iconresult.append(pixmap);

             QFileInfo f(pathresult.at(i));
             QDateTime createTime=f.created();
             m_timeResult.append(createTime.toString());
         }else{
             return;
         }



     }
     Q_EMIT requestUpdateSignal(runresult.count());
     showresult.clear();
     if(returnresult.size()<4){
         showresult=returnresult;
     }else{
         for (int h=0;h<3;h++) {
             showresult.append(returnresult.at(h));
         }
         showresult.append(QString::fromLocal8Bit("更多"));
     }

     m_showMap.insert(QString::fromLocal8Bit("FileName"),showresult);
     m_showMap.insert(QString::fromLocal8Bit("PathName"),runresult);
     m_showMap.insert(QString::fromLocal8Bit("FileTime"),m_timeResult);
    matchesChanged();
}

void filemodel::run(int row,int column){
    if(row<3){
        QString file;
        QString m_szHelpDoc;
        if(column==0){
            file=runresult.at(row);
            m_szHelpDoc = QString(QString::fromLocal8Bit("file:///")) + file;
        }else{
            file=openPathresult.at(row);
            m_szHelpDoc = QString(QString::fromLocal8Bit("file://")) + file;
        }
        QString  cmd= QString(QString::fromLocal8Bit("xdg-open "))+ m_szHelpDoc;
        QProcess app;
        app.setProgram(cmd);
        app.startDetached(app.program());
        app.waitForFinished(-1);
    }else{
        struct passwd *pwd;
        pwd=getpwuid(getuid());
        QString str = QString("search:///search_uris=file:///home/%1&name_regexp=%2&recursive=1").arg(pwd->pw_name).arg(peonyText);
        QProcess p;
        p.setProgram(QString("peony"));
        p.setArguments(QStringList()<<str);
        p.startDetached(p.program(), p.arguments());
        p.waitForFinished(-1);
    }
}


