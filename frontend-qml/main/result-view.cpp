//
// Created by hxf on 22-5-23.
//

#include "result-view.h"
#include "search-plugin-model.h"
#include "search-result-model.h"
#include "result-model-manager.h"
#include "search-worker-manager.h"

#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlError>

using namespace UkuiSearch;

ResultView::ResultView(ResultModelManager *resultModelManager) : QQuickView(), m_resultModelManager(resultModelManager)
{
    initProperty();
    initUI();
    initConnections();
}

void ResultView::initUI()
{
    setMinimumHeight(500);
    setMinimumWidth(700);
    setHeight(500);
    setWidth(700);

    setResizeMode(ResizeMode::SizeRootObjectToView);
    setSource(QUrl("qrc:/qml/ResultView.qml"));
}

void ResultView::initConnections()
{

}

void ResultView::initProperty()
{
    //注册元类型使得qml可以识别
    qRegisterMetaType<SearchPluginModel*>("SearchPluginModel*");

    m_searchPluginModel = new SearchPluginModel(this);
    rootContext()->setContextProperty("pluginModel", m_searchPluginModel);

    rootContext()->setContextProperty("resultModelManager", m_resultModelManager);
}
