//
// Created by hxf on 22-6-13.
//

#include "details-utils.h"
#include "result-model-manager.h"
#include "search-result-model.h"
#include "search-plugin-model.h"
#include "search-plugin-manager.h"

#include <QDebug>

using namespace UkuiSearch;

DetailsUtils::DetailsUtils(QObject *parent) : QObject(parent)
{

}

QVariant DetailsUtils::getPluginData(const QString &pluginId, const QString &name)
{
    SearchPluginModel *pluginModel = SearchPluginModel::getInstance();

    int index = pluginModel->getPlugins().indexOf(pluginId);
    if (index >= 0) {
        int role = pluginModel->roleNames().key(name.toUtf8());

        return pluginModel->data(pluginModel->index(index), role);
    }

    return {};
}

QVariant DetailsUtils::getResultData(const QString &pluginId, int index, const QString &name)
{
    SearchResultModel *resultModel = ResultModelManager::getInstance()->getModel(pluginId);

    if (resultModel) {
        int role = resultModel->roleNames().key(name.toUtf8());

        return resultModel->data(resultModel->index(index), role);
    }

    return {};
}

QString DetailsUtils::getPluginActions(const QString &pluginId, int type)
{
    SearchPluginIface *plugin = SearchPluginManager::getInstance()->getPlugin(pluginId);
    QStringList objs;

    if (plugin) {
        QList<SearchPluginIface::Actioninfo> actions = plugin->getActioninfo(type);

        for (const auto &item: actions) {
            objs.append(QString(R"({"k":%1,"v":"%2"})").arg(item.actionkey).arg(item.displayName));
        }
    }

    QString desc = QString(R"({"keys":[%1]})").arg(objs.join(","));

    return desc;
}

void DetailsUtils::openAction(const QString &pluginId, int actionKey, const QString& key, int type)
{
    SearchPluginIface *plugin = SearchPluginManager::getInstance()->getPlugin(pluginId);

    if (plugin) {
        plugin->openAction(actionKey, key, type);
    }
}
