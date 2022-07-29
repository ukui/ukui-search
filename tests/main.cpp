#include <QCoreApplication>
#include "file-system-watcher-test.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    FileSystemWatcherTest test;
    test.beginSignalTest();

    return a.exec();
}
