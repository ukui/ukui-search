//
// Created by hxf on 22-5-23.
//

#include "result-view.h"
#include "search-plugin-model.h"
#include "result-model-manager.h"
#include "search-worker-manager.h"
#include "model-data-provider.h"

#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlError>

using namespace UkuiSearch;

ResultView::ResultView() : QQuickView()
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

    m_searchPluginModel = SearchPluginModel::getInstance(this);
    rootContext()->setContextProperty("pluginModel", m_searchPluginModel);

    m_resultModelManager = ResultModelManager::getInstance(this);
    rootContext()->setContextProperty("resultModelManager", m_resultModelManager);
}

void ResultView::installDataProvider(ModelDataProvider *provider)
{
    if (m_resultModelManager) {
        m_resultModelManager->installDataProvider(provider);
    }
}
