#include "filtermodel.h"
#include <QDebug>

appfiltermodel::appfiltermodel()
{
      runnerModel = new RunnerModel(this);
      appct=0;
}

bool appfiltermodel::filterAcceptsRow(int source_row,const QModelIndex & source_parent)const{
    QModelIndex source_index = sourceModel()->index(source_row, 0, source_parent);
 if(sourceModel()->data(source_index,runnerModel->RunnerId)==QString::fromLocal8Bit("services")){
     return true;
 }
     return false;
}

QVariant appfiltermodel::headerData(int section,Qt::Orientation orientation ,int role) const {
    if(role == 0){
        return tr("App");
    }
    return QAbstractItemModel::headerData(section,orientation,role);
}

filefiltermodel::filefiltermodel()
{
     runnerModel1 = new RunnerModel(this);
}

bool filefiltermodel::filterAcceptsRow(int source_row,const QModelIndex & source_parent)const{
    QModelIndex source_index = sourceModel()->index(source_row, 0, source_parent);
 if(sourceModel()->data(source_index,runnerModel1->RunnerId)==QString::fromLocal8Bit("baloosearch")){
     return true;
 }
     return false;
}

QVariant filefiltermodel::headerData(int section,Qt::Orientation orientation ,int role) const {
    if(role == 0){
        return tr("File");
    }
    return QAbstractItemModel::headerData(section,orientation,role);
}


