#include "mainwindow.h"
#include "inotify-manager.h"
#include <QTime>
#include <QDebug>

void testTraverse(void){


//    QStringList qsl;
//    for (int i = 0; i < 4000; i++){
//        qsl.append(QString("%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1%1").arg(i));
//    }

//    for (int i = 0; i < 4000; i++){
//        qDebug() << i << " " << qsl.at(i);
//        if (qsl.at(i) != QString("%1").arg(i)){
//            qDebug() << "fuck stringlist";
//        }
//    }

//    qDebug() << qsl.length();
//    qDebug() << "stringlist";
//    exit(0);




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
