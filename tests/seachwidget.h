#ifndef SEACHWIDGET_H
#define SEACHWIDGET_H

#include <QObject>
#include <QWidget>
#include <QAction>
#include <QApplication>
#include <QLineEdit>
#include <QTreeView>
#include <QVBoxLayout>
#include "runmodel/runnermodel.h"

class seachwidget : public QWidget
{
    Q_OBJECT
public:
    explicit seachwidget(QWidget *parent = nullptr);

//signals:

};

#endif // SEACHWIDGET_H
