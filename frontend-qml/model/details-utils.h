//
// Created by hxf on 22-6-13.
//

#ifndef UKUI_SEARCH_DETAILS_UTILS_H
#define UKUI_SEARCH_DETAILS_UTILS_H

#include <QObject>
#include <QVariant>

namespace UkuiSearch {

class DetailsUtils : public QObject
{
    Q_OBJECT
public:
    explicit DetailsUtils(QObject *parent = nullptr);

    //获取插件的信息
    Q_INVOKABLE QVariant getPluginData(const QString& pluginId, const QString& name);

    //获取搜索结果数据信息
    Q_INVOKABLE QVariant getResultData(const QString& pluginId, int index, const QString& name);

    //插件的action
    Q_INVOKABLE QString getPluginActions(const QString& pluginId, int type);

    Q_INVOKABLE void openAction(const QString& pluginId, int actionKey, const QString& key, int type);

};

} // UkuiSearch

#endif //UKUI_SEARCH_DETAILS_UTILS_H
