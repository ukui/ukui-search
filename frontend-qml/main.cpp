#include <QApplication>
#include <QWindow>
#include <QQuickView>
#include <QUrl>
#include <QScreen>
#include <QCursor>
#include "main-window.h"
#include "search-application.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    SearchApplication app(argc, argv);
    if (app.isRunning())
        return 0;

    return SearchApplication::exec();
}
