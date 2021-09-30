#ifndef WEBSEARCHPAGE_H
#define WEBSEARCHPAGE_H

#include <QObject>
#include <QWidget>
#include <QBoxLayout>
#include "web-search-view.h"
namespace Zeeker {
class WebSearchPage : public QWidget
{
    Q_OBJECT
public:
    explicit WebSearchPage(QWidget *parent = nullptr);

    //bool isSelected();
    void LaunchBrowser();
    void clearResultSelection();

protected:
    void paintEvent(QPaintEvent *event);

private:
    void initUI();
    void initConnections();

    QVBoxLayout * m_mainLyt = nullptr;
    WebSearchWidget * m_webSearchWidget = nullptr;

Q_SIGNALS:
    void startSearch(const QString &);

public Q_SLOTS:
    void setWebSearchSelection();

};
}

#endif // WEBSEARCHPAGE_H
