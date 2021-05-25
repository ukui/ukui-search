#ifndef RESULTVIEW_H
#define RESULTVIEW_H
#include <QTreeView>
#include <QVBoxLayout>
#include <QLabel>
#include "search-result-model.h"
#include "show-more-label.h"
#include "title-label.h"
#include "result-view-delegate.h"

namespace Zeeker {

class ResultView : public QTreeView
{
    Q_OBJECT
public:
    ResultView(const QString &plugin_id, QWidget *parent = nullptr);
    ~ResultView() = default;
    bool isSelected();

public Q_SLOTS:
    void clearSelectedRow();
    void onRowDoubleClickedSlot(const QModelIndex &);
    void onRowSelectedSlot(const QItemSelection &, const QItemSelection &);

private:
    void initConnections();
    SearchResultModel * m_model = nullptr;
    QString m_plugin_id;
    bool m_is_selected = false;
    ResultViewDelegate * m_style_delegate = nullptr;

Q_SIGNALS:
    void startSearch(const QString &);
    void stopSearch();
    void currentRowChanged(const QString &, const SearchPluginIface::ResultInfo&);

};

class ResultWidget : public QWidget
{
    Q_OBJECT
public:
    ResultWidget(const QString &plugin_id, QWidget *parent = nullptr);
    ~ResultWidget() = default;
    QString pluginId();
    void setEnabled(const bool&);

public Q_SLOTS:
    void expandListSlot();
    void reduceListSlot();

private:
    QString m_plugin_id;
    bool m_enabled = true;

    void initUi();
    void initConnections();
    QVBoxLayout * m_mainLyt = nullptr;
    TitleLabel * m_titleLabel = nullptr;
    ResultView * m_resultView = nullptr;
    ShowMoreLabel * m_showMoreLabel = nullptr;

Q_SIGNALS:
    void startSearch(const QString &);
    void stopSearch();
    void currentRowChanged(const QString &, const SearchPluginIface::ResultInfo&);
    void clearSelectedRow();
};
}

#endif // RESULTVIEW_H
