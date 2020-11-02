#include "seachwidget.h"
#include "filtermodel.h"

#include <QDebug>
#include <QListView>
#include <QString>
#include <QSortFilterProxyModel>
#include <QStandardItem>
#include <QLabel>


seachwidget::seachwidget(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    RunnerModel *runnerModel = new RunnerModel(this);
    QLineEdit *input = new QLineEdit(this);
    connect(input, &QLineEdit::textChanged, runnerModel, &RunnerModel::scheduleQuery);
    layout->addWidget(input);

    QTreeView *view1 = new QTreeView(this);
    appfiltermodel *mAppModel = new appfiltermodel;
    mAppModel->setSourceModel(runnerModel);
    view1->setModel(mAppModel);


    QTreeView *view2 = new QTreeView(this);
    filefiltermodel *mFileModel = new filefiltermodel;
    mFileModel->setSourceModel(runnerModel);
    view2->setModel(mFileModel);


    QTreeView *view3 = new QTreeView(this);
    view3->setModel(runnerModel);

    QLabel *label4 =new QLabel(this);



    connect(runnerModel,&RunnerModel::runnersChanged,this,[=](){

    });
    connect(runnerModel,&RunnerModel::runningChanged,this,[=](bool finish){

    });
    connect(mAppModel,&appfiltermodel::rowsMoved,this,[=](){
        qDebug()<<view1->indentation();
    });
    connect(view1,&QTreeView::clicked,this,[=](){
        runnerModel->run(view1->currentIndex().row());
    });
    connect(view2,&QTreeView::clicked,this,[=](){
        runnerModel->run(mFileModel->mapToSource(view2->currentIndex()).row());
    });

    layout->addWidget(view1);
    layout->addWidget(view2);
    layout->addWidget(view3);
    layout->addWidget(label4);
}

