#ifndef BESTLISTVIEW_H
#define BESTLISTVIEW_H
#include <QTreeView>
#include <QVBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QApplication>
#include "best-list-model.h"
#include "show-more-label.h"
#include "title-label.h"
#include "result-view-delegate.h"

namespace Zeeker {

class BestListView : public QTreeView
{
    Q_OBJECT
public:
    BestListView(QWidget *parent = nullptr);
    ~BestListView() = default;
    bool isSelected();
    int showHeight();

public Q_SLOTS:
    void clearSelectedRow();
    void onRowDoubleClickedSlot(const QModelIndex &);
    void onRowSelectedSlot(const QItemSelection &, const QItemSelection &);
    void onItemListChanged(const int &);
    void setExpanded(const bool &);
    const bool &isExpanded();
    void onMenuTriggered(QAction *);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    void initConnections();
    BestListModel * m_model = nullptr;
    bool m_is_selected = false;
    ResultViewDelegate * m_style_delegate = nullptr;
    int m_count = 0;

Q_SIGNALS:
    void startSearch(const QString &);
    void currentRowChanged(const QString &, const SearchPluginIface::ResultInfo&);
    void sendBestListData(const QString &, const SearchPluginIface::ResultInfo&);
    void listLengthChanged(const int &);
    void rowClicked();

};


class BestListWidget : public QWidget
{
    Q_OBJECT
public:
    BestListWidget(QWidget *parent = nullptr);
    ~BestListWidget() = default;
    void setEnabled(const bool&);

public Q_SLOTS:
    void expandListSlot();
    void reduceListSlot();
    void onListLengthChanged(const int &);

private:
    bool m_enabled = true;

    void initUi();
    void initConnections();
    QVBoxLayout * m_mainLyt = nullptr;
    TitleLabel * m_titleLabel = nullptr;
    BestListView * m_bestListView = nullptr;
    ShowMoreLabel * m_showMoreLabel = nullptr;

Q_SIGNALS:
    void startSearch(const QString &);
    void stopSearch();
    void currentRowChanged(const QString &, const SearchPluginIface::ResultInfo&);
    void sendBestListData(const QString &, const SearchPluginIface::ResultInfo&);
    void clearSelectedRow();
    void sizeChanged();
    void rowClicked();
};

}

#endif // BESTLISTVIEW_H
