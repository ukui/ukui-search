#ifndef SEARCHLISTVIEW_H
#define SEARCHLISTVIEW_H

#include <QObject>
#include <QTreeView>
#include "model/search-item-model.h"
#include "model/search-item.h"

class SearchListView : public QTreeView
{
    Q_OBJECT
public:
    explicit SearchListView(QWidget *, const QStringList&, int);
    ~SearchListView();

private:
    SearchItemModel * m_model = nullptr;
    SearchItem * m_item = nullptr;
};

#endif // SEARCHLISTVIEW_H
