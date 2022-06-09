#include <QApplication>
#include <QQmlApplicationEngine>
#include <QWindow>
#include <QQuickView>
#include <QUrl>
#include <QScreen>
#include <QCursor>
#include "main-window.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication app(argc, argv);

    UkuiSearch::MainWindow *mainWindow = new UkuiSearch::MainWindow();
    mainWindow->showWindow();

    return app.exec();
}
