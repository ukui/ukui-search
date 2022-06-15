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

namespace UkuiSearch {

class BestListView : public QTreeView
{
    Q_OBJECT
public:
    BestListView(QWidget *parent = nullptr);
    ~BestListView() = default;

    bool isSelected();
    int showHeight();
    int getResultNum();
    QModelIndex getModlIndex(int row, int column);
    SearchPluginIface::ResultInfo getIndexResultInfo(QModelIndex &index);
    const QString getPluginInfo(const QModelIndex&index);
    int getResultHeight();

public Q_SLOTS:
    void clearSelectedRow();
    void onRowDoubleClickedSlot(const QModelIndex &);
    void onRowSelectedSlot(const QModelIndex &index);
    void onItemListChanged(const int &);
    void setExpanded(const bool &);
    const bool &isExpanded();
    void onMenuTriggered(QAction *);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    void initConnections();

    BestListModel * m_model = nullptr;
    bool m_is_selected = false;
    ResultViewDelegate * m_styleDelegate = nullptr;
    int m_count = 0;
    QModelIndex m_tmpCurrentIndex;
    QModelIndex m_tmpMousePressIndex;

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

    QString getWidgetName();
    void setEnabled(const bool&);
    void clearResult();
    int getResultNum();
    void setResultSelection(const QModelIndex &index);
    void clearResultSelection();
    QModelIndex getModlIndex(int row, int column);
    void activateIndex();
    QModelIndex getCurrentSelection();
    bool getExpandState();
    SearchPluginIface::ResultInfo getIndexResultInfo(QModelIndex &index);
    const QString getPluginInfo(const QModelIndex&index);
    int getResultHeight();

private:
    void initUi();
    void initConnections();

    bool m_enabled = true;
    QVBoxLayout * m_mainLyt = nullptr;
    TitleLabel * m_titleLabel = nullptr;
    BestListView * m_bestListView = nullptr;

public Q_SLOTS:
    void expandListSlot();
    void reduceListSlot();
    void onListLengthChanged(const int &);

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
