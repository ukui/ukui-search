#include <QAction>
#include <QApplication>
#include <QLineEdit>
#include <QTreeView>
#include <QVBoxLayout>
#include "seachwidget.h"
#include "runmodel/runnermodel.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    seachwidget *widget = new seachwidget;
    widget->show();
    return app.exec();
}
