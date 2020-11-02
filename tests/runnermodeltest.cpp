#include <QAction>
#include <QApplication>

#include "mainrunner.h"



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    mainrunner xxx;
    xxx.show();
    xxx.move ((QApplication::desktop()->width() - xxx.width())/2,(QApplication::desktop()->height() - xxx.height())/2);

    return app.exec();
}
