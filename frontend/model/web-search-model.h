#ifndef WEBSEARCHMODEL_H
#define WEBSEARCHMODEL_H

#include <QAbstractItemModel>
#include <QIcon>
#include <QLabel>
#include "search-result-model.h"

namespace UkuiSearch {
class WebSearchModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit WebSearchModel(QObject *parent = nullptr);

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
public Q_SLOTS:
    void startSearch(const QString &);

private:
    SearchResultItem * m_item = nullptr;
};
}
#endif // WEBSEARCHMODEL_H
