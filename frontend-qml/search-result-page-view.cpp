#include "search-result-page-view.h"
#include <KWindowEffects>
#include <KWindowSystem>
#include <QQmlEngine>
#include <QQmlContext>
#include <QScopedPointer>
#include "search-plugin-manager.h"
#include "search-result-model.h"

SearchResultPageView::SearchResultPageView() : QQuickView()
{
    setResizeMode(QQuickView::SizeViewToRootObject);
    setColor(Qt::transparent);
    KWindowEffects::enableBlurBehind(this, true, QRegion());
    KWindowSystem::setState(this->winId(),NET::SkipTaskbar | NET::SkipPager | NET::SkipSwitcher );
    qRegisterMetaType<UkuiSearch::SearchPluginIface::ResultInfo>("SearchPluginIface::ResultInfo");


    qmlRegisterSingletonType<UkuiSearch::SearchPluginManager>("ukui.search", 1, 0, "SearchPluginManager", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject *{
       return UkuiSearch::SearchPluginManager::getInstance();});
//    qmlRegisterType<UkuiSearch::SearchResultModel>("ukui.search", 1, 0, "SearchResultModel");
//    QScopedPointer<UkuiSearch::SearchResultModel> m(new UkuiSearch::SearchResultModel());
//    UkuiSearch::SearchResultModel m;
//    m.get()->setPluginId("File Search");
//    m.get()->startSearch("1");
    setResizeMode(QQuickView::SizeRootObjectToView);
//    setInitialProperties({{"model", QVariant::fromValue(m.get())}});
    setSource(QUrl(QStringLiteral("qrc:/SearchResultPage.qml")));
}

void SearchResultPageView::textChangeSlot(QString text)
{
//    qDebug() << text;
    reinterpret_cast<QObject*>(this->rootObject())->setProperty("keyword", text);
//    reinterpret_cast<QObject*>(this->rootObject())->property("keyword").toString();
}
