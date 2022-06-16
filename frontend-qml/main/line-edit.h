//
// Created by hxf on 22-5-20.
//

#ifndef UKUI_SEARCH_LINE_EDIT_H
#define UKUI_SEARCH_LINE_EDIT_H

#include <QQuickView>

namespace UkuiSearch {

class ModelDataProvider;

class LineEdit : public QQuickView
{
    Q_OBJECT
public:
    explicit LineEdit();

    //安装数据提供器
    void installDataProvider(ModelDataProvider *provider);

private:
    void initUI();
    void initConnections();

private Q_SLOTS:
    void onTextChanged(const QString &keyword);

Q_SIGNALS:
    void keywordChanged();

private:
    QHash<QString, ModelDataProvider*> m_providers;
};

} // UkuiSearch

#endif //UKUI_SEARCH_LINE_EDIT_H
