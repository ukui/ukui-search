#include "src/mainwindow.h"
#include "inotify-manager.h"
#include <QTime>
#include <QDebug>

void testTraverse(void){
    QTime t1 = QTime::currentTime();
    InotifyManager* im = new InotifyManager();
    im->AddWatch("/home/zpf");
    im->Traverse_BFS("/home/zpf", true);
    QTime t2 = QTime::currentTime();
    qDebug() << t1;
    qDebug() << t2;
    im->start();

    //exit(0);
}
