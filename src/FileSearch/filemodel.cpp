#include "filemodel.h"
#include <stdio.h>
#include <pwd.h>
#include <unistd.h>

/**
 * @brief filemodel::filemodel 显示文件结果的model
 */
filemodel::filemodel():
    fileutils(new FileUtils)
{
}

/**
 * @brief filemodel::rowCount 重写的model行数函数
 * @param index 条目的索引
 * @return model显示的行数
 */
int filemodel::rowCount(const QModelIndex& index) const
{
    return index.isValid() ? 0 : showresult.count();
}

/**
 * @brief filemodel::columnCount 重写的model列数函数
 * @param index 条目的索引
 * @return model显示的列数
 */
int filemodel::columnCount(const QModelIndex& index) const
{
    return index.isValid() ? 0 :3;
}

/**
 * @brief filemodel::headerData filemodel::columnCount 重写的model标头函数
 * @param section 列
 * @param orientation 显示方式
 * @param role 显示内容类型
 * @return 标头数据
 */
QVariant filemodel::headerData(int section,Qt::Orientation orientation ,int role) const {
    if(role == Qt::DisplayRole&&orientation==Qt::Horizontal){
        return header[section];
    }
    return QAbstractItemModel::headerData(section,orientation,role);
}

/**
 * @brief filemodel::data model每条条目的数据，有显示内容，图片，内容字体字号等
 * @param index 条目索引
 * @param role 显示内容的类型
 * @return 显示内容数据
 */
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
//    case Qt::FontRole:
//        if(index.column()==0){
//            return QFont("宋体",12,QFont::Bold);
//        }else{
//            return QFont("黑体",8,QFont::Bold);
//        }
    case Qt::SizeHintRole:
        return QSize(200,40);
    case Qt::TextColorRole:
        if(index.column()==0){
//            return QColor(Qt::black);
        }
    }
return QVariant();
}

/**
 * @brief filemodel::matchstart 匹配前将model中各成员变量进行初始化
 * @param source 现在无意义
 */
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

/**
 * @brief filemodel::matchesChanged 更新model内容
 */
void filemodel::matchesChanged()
{
        beginResetModel();
        endResetModel();
}

/**
 * @brief filemodel::showResult 将路径等数据分别给各成员变量
 * @param result
 */
int filemodel::showResult(QStringList result){
    matchstart(peonyText);
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

             //添加文件图标
              QString str= QString::fromLocal8Bit("file://")+pathresult.at(i);
             if(str1.at(str1.count()-1).contains("#")){
                 QString str3=str1.at(str1.count()-1);
                 str3.replace("#","%23");
                 str="file://"+str2+"/"+str3;
             }
             QIcon icon= QIcon::fromTheme(fileutils->getFileIconName(str));
             if(icon.isNull()){
                 icon= QIcon::fromTheme("text-plain");
             }
             QPixmap pixmap = icon.pixmap(QSize(30,30));
             iconresult.append(pixmap);

             //添加文件创建时间
             QFileInfo f(pathresult.at(i));
             QDateTime createTime=f.created();
             QStringList createTimeList=createTime.toString(Qt::ISODate).split("T");
             m_timeResult.append(createTimeList.at(0));
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
         showresult.append(QString::fromLocal8Bit("显示更多"));
     }

     m_showMap.insert(QString::fromLocal8Bit("FileName"),showresult);
     m_showMap.insert(QString::fromLocal8Bit("PathName"),runresult);
     m_showMap.insert(QString::fromLocal8Bit("FileTime"),m_timeResult);
    matchesChanged();
    return showresult.size();
}

/**
 * @brief filemodel::run 运行点击条目对应的文件
 * @param row 条目行数
 * @param column 条目列数
 */
void filemodel::run(int row,int column){
    if(row<3){
        QString file;
        QString m_szHelpDoc;
        if(column==0){
            file=runresult.at(row);
            if(file.contains("#")){
                file.replace("#","%23");
            }
            m_szHelpDoc = QString(QString::fromLocal8Bit("file://")) + file;
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

