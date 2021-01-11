#ifndef SEARCHITEMMODEL_H
#define SEARCHITEMMODEL_H

#include <QObject>
#include <QAbstractItemModel>
#include "search-item.h"

class SearchItem;

class SearchItemModel : public QAbstractItemModel
{
    friend class SearchItem;
    Q_OBJECT
public:
    explicit SearchItemModel();
    ~SearchItemModel();

    enum SearchInfo {
        Icon,
        Name
    };

    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex())override;
//    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setItem(SearchItem *);

    void appendItem(QString);
    void removeItem(QString);
    void clear();

private :
    SearchItem * m_item = nullptr;
};

#endif // SEARCHITEMMODEL_H
