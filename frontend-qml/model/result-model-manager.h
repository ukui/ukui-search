//
// Created by hxf on 22-5-26.
//

#ifndef UKUI_SEARCH_RESULT_MODEL_MANAGER_H
#define UKUI_SEARCH_RESULT_MODEL_MANAGER_H

#include <QObject>
#include <QHash>
#include <QTreeView>

#include "search-plugin-manager.h"

namespace UkuiSearch {

class SearchResultModel;
class ModelDataProvider;

class ResultModelManager : public QObject
{
    Q_OBJECT
public:
    explicit ResultModelManager(QObject *parent = nullptr);

    //函数体放到头文件中是为了解决Q_INVOKABLE引起的找不到函数定义问题
    Q_INVOKABLE inline UkuiSearch::SearchResultModel *getModel(const QString &pluginId) {
        if (m_models.contains(pluginId)) {
            return m_models.value(pluginId);
        }
        return nullptr;
    }

    //展开，收起
    Q_INVOKABLE void expand(const QString &pluginId);
    Q_INVOKABLE void collapse(const QString &pluginId);

    //安装数据提供器
    void installDataProvider(ModelDataProvider *provider);

private Q_SLOTS:
    void receiveData(const SearchPluginIface::ResultInfo& data, const QString &pluginId);
    void clearModelData(const QString &pluginId);
    void clearAllModelData();

private:
    QMutex mutex;
    QHash<QString, SearchResultModel*> m_models;
    QHash<QString, ModelDataProvider*> m_providers;
};

} // UkuiSearch

#endif //UKUI_SEARCH_RESULT_MODEL_MANAGER_H
