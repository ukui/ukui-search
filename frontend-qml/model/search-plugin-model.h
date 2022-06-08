//
// Created by hxf on 22-5-24.
//

#ifndef UKUI_SEARCH_SEARCH_PLUGIN_MODEL_H
#define UKUI_SEARCH_SEARCH_PLUGIN_MODEL_H

#include <QAbstractListModel>

namespace UkuiSearch {

class SearchPluginManager;

class SearchPluginModel : public QAbstractListModel
{
public:
    enum {
        IdRole = Qt::UserRole + 1,
        NameRole,
        IconRole,
        IconNameRole,
        DescriptionRole,
        DetailsDescRole
    };

    explicit SearchPluginModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QHash<int, QByteArray> roleNames() const override;

private:
    SearchPluginManager *m_searchPluginManager = nullptr;
    QVector<QString> m_plugins;
    QHash<int, QByteArray> m_roleNames;
};

} // UkuiSearch

#endif //UKUI_SEARCH_SEARCH_PLUGIN_MODEL_H
