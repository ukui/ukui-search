#include <QApplication>
#include <QQmlApplicationEngine>
#include <QWindow>
#include <KWindowEffects>
#include <QPainterPath>
#include <QRegion>
#include <QQuickView>
#include <QUrl>
#include <QScreen>
#include <QCursor>
#include "xatom-helper.h"
#include "search-line-edit-view.h"
#include "search-result-page-view.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication app(argc, argv);

//    QQmlApplicationEngine engine;
//    const QUrl url(QStringLiteral("qrc:/main.qml"));
//    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
//                     &app, [url](QObject *obj, const QUrl &objUrl) {
//        if (!obj && url == objUrl)
//            QCoreApplication::exit(-1);
//    }, Qt::QueuedConnection);
//    engine.load(url);


    SearchLineEditView *searchLineEditView = new SearchLineEditView;


    MotifWmHints hints;
    hints.flags = MWM_HINTS_FUNCTIONS|MWM_HINTS_DECORATIONS;
    hints.functions = MWM_FUNC_ALL;
    hints.decorations = MWM_DECOR_BORDER;
    WId winid = searchLineEditView->winId();
    XAtomHelper::getInstance()->setWindowMotifHint(winid, hints);


    searchLineEditView->showWindow();


    SearchResultPageView *searchResultPageView = new SearchResultPageView;

    searchResultPageView->setPosition(searchLineEditView->x(), searchLineEditView->y() + 58 );
    searchResultPageView->show();
    XAtomHelper::getInstance()->setWindowMotifHint(searchResultPageView->winId(), hints);

    QObject::connect(searchLineEditView, &SearchLineEditView::textChange, searchResultPageView, &SearchResultPageView::textChangeSlot);


    return app.exec();
}
