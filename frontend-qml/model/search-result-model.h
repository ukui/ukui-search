//
// Created by hxf on 22-5-26.
//

#ifndef UKUI_SEARCH_SEARCH_RESULT_MODEL_H
#define UKUI_SEARCH_SEARCH_RESULT_MODEL_H

#include <QAbstractListModel>
#include <QIcon>

#include "search-plugin-iface.h"

namespace UkuiSearch {

class SearchResultModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(NOTIFY dataLoaded)
public:
    enum DataRoles {
        IconRole = Qt::UserRole + 1,
        IconNameRole,
        NameRole,
        DescriptionRole
    };

    explicit SearchResultModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QHash<int, QByteArray> roleNames() const override;

    ~SearchResultModel() override;

private:
    inline int virtualRowCount() const;
    static QString generateDesc(const SearchPluginIface::ResultInfo& data) ;

public Q_SLOTS:
    void clear();
    void expand();
    void collapse();
    void insertData(const SearchPluginIface::ResultInfo &data);

Q_SIGNALS:
    void dataLoaded();

private:
    QVector<SearchPluginIface::ResultInfo> m_list;
    QHash<int, QByteArray> m_roleNamesHash;

    bool m_expanded = false;
    const int m_minRowCount = 5; //不展开状态最多只显示5个结果
    const int m_maxRowCount = 100; //最多展示100条记录
};

} // UkuiSearch

#endif //UKUI_SEARCH_SEARCH_RESULT_MODEL_H
