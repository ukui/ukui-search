//
// Created by hxf on 22-5-26.
//

#include "result-model-manager.h"
#include "search-result-model.h"
#include "model-data-provider.h"

#include <QDebug>

using namespace UkuiSearch;

static ResultModelManager *instance = nullptr;

ResultModelManager *ResultModelManager::getInstance(QObject *parent)
{
    if (!instance) {
        instance = new ResultModelManager(parent);
    }
    return instance;
}

ResultModelManager::ResultModelManager(QObject *parent) : QObject(parent)
{
    //注册元类型使得qml可以识别
    qRegisterMetaType<SearchResultModel*>("SearchResultModel*");

    QStringList list = SearchPluginManager::getInstance()->getPluginIds();
    for (const QString &item: list) {
        m_models.insert(item, new SearchResultModel(this));
    }
}

void ResultModelManager::installDataProvider(ModelDataProvider *provider)
{
    if (provider && !m_providers.contains(provider->id()) && provider->getRepeater()) {
        m_providers.insert(provider->id(), provider);

        connect(provider->getRepeater(), &ProviderRepeater::sendData,
                this, &ResultModelManager::receiveData, Qt::QueuedConnection);

        connect(provider->getRepeater(), &ProviderRepeater::resetKeywords,
                this, &ResultModelManager::clearAllModelData, Qt::QueuedConnection);
    }
}

void ResultModelManager::collapse(const QString &pluginId)
{
    QMutexLocker locker(&mutex);
    SearchResultModel *model = getModel(pluginId);
    if (model) {
        m_models.value(pluginId)->collapse();
    }
}

void ResultModelManager::expand(const QString &pluginId)
{
    QMutexLocker locker(&mutex);
    SearchResultModel *model = getModel(pluginId);
    if (model) {
        m_models.value(pluginId)->expand();
    }
}

void ResultModelManager::receiveData(const SearchPluginIface::ResultInfo &data, const QString &pluginId)
{
    QMutexLocker locker(&mutex);
    SearchResultModel *model = getModel(pluginId);
    if (model) {
        model->insertData(data);
    }
}

void ResultModelManager::clearModelData(const QString &pluginId)
{
    QMutexLocker locker(&mutex);
    SearchResultModel *model = getModel(pluginId);
    if (model) {
        model->clear();
    }
}

void ResultModelManager::clearAllModelData()
{
    QMutexLocker locker(&mutex);
    for (const auto &model: m_models) {
        model->clear();
    }
}
