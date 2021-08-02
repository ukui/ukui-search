#ifndef BESTLISTMODEL_H
#define BESTLISTMODEL_H

#include <QAbstractItemModel>
#include "search-result-model.h"

#define NUM_LIMIT_SHOWN_DEFAULT 5

namespace Zeeker {
class BestListModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit BestListModel(QObject *parent = nullptr);

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    const SearchPluginIface::ResultInfo & getInfo(const QModelIndex&);
    const QString & getPluginInfo(const QModelIndex&);

    void setExpanded(const bool&);
    const bool &isExpanded();

    QStringList getActions(const QModelIndex &);
    QString getKey(const QModelIndex &);

public Q_SLOTS:
    void appendInfo(const QString &, const SearchPluginIface::ResultInfo &);
    void startSearch(const QString &);

Q_SIGNALS:
    void stopSearch();
    void itemListChanged(const int&);

private:
    void initConnections();
    SearchResultItem * m_item = nullptr;
    QVector<QString> m_plugin_id_list;
    bool m_isExpanded = false;
};
}
#endif // BESTLISTMODEL_H
