#include "windowshow.h"

#include <QLineEdit>
#include <QTreeView>
#include <QVBoxLayout>

#include "declarative/runnermodel.h"

#include <QDebug>

windowshow::windowshow(QObject *parent)
    : QObject(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    RunnerModel *runnerModel = new RunnerModel(this);

    QLineEdit *input = new QLineEdit(this);
    connect(input, &QLineEdit::textChanged, runnerModel, &RunnerModel::scheduleQuery);
    layout->addWidget(input);

    QTreeView *view = new QTreeView(this);

    view->setModel(runnerModel);

    layout->addWidget(view);

    connect(view,&QTreeView::clicked,this,[=](){
        //runnerModel->run(view->currentIndex().row());
       // qDebug()<<runnerModel->roleNames();
        qDebug()<<runnerModel->data(view->currentIndex(),runnerModel->RunnerId).toString();
        //qDebug()<<runnerModel->RunnerId;
    });

    this->setFixedSize(400,400);

    QLabel *xxxx=new QLabel(this);
    xxxx->setText("dsafsaf");
}
