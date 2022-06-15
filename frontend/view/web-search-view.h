#ifndef WEBSEARCHVIEW_H
#define WEBSEARCHVIEW_H
#include <QTreeView>
#include <QObject>
#include <QListView>
#include <QMouseEvent>
#include "web-search-model.h"
#include "web-search-view-delegate.h"
#include "title-label.h"

namespace Zeeker {
class WebSearchView : public QTreeView
{
    Q_OBJECT
public:
    WebSearchView(QWidget *parent = nullptr);
    ~WebSearchView() = default;

    bool isSelected();
    int showHeight();
    QModelIndex getModlIndex(int row, int column);
    void LaunchBrowser();

public Q_SLOTS:
    void clearSelectedRow();
    void startSearch(const QString &);

protected:
    void mouseReleaseEvent(QMouseEvent *event);

private:
    void initConnections();

    WebSearchModel * m_model = nullptr;
    bool m_is_selected = false;
    WebSearchViewDelegate * m_styleDelegate = nullptr;
    QString m_keyWord;
};

class WebSearchWidget : public QWidget
{
    Q_OBJECT
public:
    WebSearchWidget(QWidget *parent = nullptr);
    ~WebSearchWidget() = default;

    QString getWidgetName();
    void setEnabled(const bool&);
    void clearResultSelection();
    QModelIndex getModlIndex(int row, int column);
    void setResultSelection(const QModelIndex &index);
    void LaunchBrowser();
    bool isSelected();

private:
    void initUi();
    void initConnections();

    bool m_enabled = true;
    QVBoxLayout * m_mainLyt = nullptr;
    QHBoxLayout * m_resultLyt = nullptr;
    //TitleLabel * m_titleLabel = nullptr;
    WebSearchView * m_webSearchView = nullptr;
    QLabel * m_queryIcon = nullptr;

Q_SIGNALS:
    void startSearch(const QString &);
    void clearSelectedRow();
    void rowClicked();

};

}
#endif // WEBSEARCHVIEW_H
