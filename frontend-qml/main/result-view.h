//
// Created by hxf on 22-5-23.
//

#ifndef UKUI_SEARCH_RESULT_VIEW_H
#define UKUI_SEARCH_RESULT_VIEW_H

#include <QQuickView>

namespace UkuiSearch {

class SearchPluginModel;
class ResultModelManager;
class ModelDataProvider;

class ResultView : public QQuickView
{
    Q_OBJECT
public:
    explicit ResultView();

    //安装数据提供器
    void installDataProvider(ModelDataProvider *provider);

private:
    void initUI();
    void initConnections();
    void initProperty();

private:
    SearchPluginModel *m_searchPluginModel = nullptr;
    ResultModelManager *m_resultModelManager = nullptr;
};

}

#endif //UKUI_SEARCH_RESULT_VIEW_H
