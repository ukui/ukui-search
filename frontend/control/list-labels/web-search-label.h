#ifndef WEBSEARCHLABEL_H
#define WEBSEARCHLABEL_H
#include <QLabel>
#include "title-label.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace Zeeker {
class WebSearchLabel : public QLabel
{
    Q_OBJECT
public:
    WebSearchLabel(QWidget * parent = nullptr);
    ~WebSearchLabel() = default;

protected:
    bool eventFilter(QObject *watched, QEvent *event);
private:
    void initUi();
    void initConnections();
    void startSearch();

    QHBoxLayout * m_webSearchLyt = nullptr;
    QLabel * m_WebSearchIconlabel = nullptr;
    QLabel * m_WebSearchLabel = nullptr;

public Q_SLOTS:
    void webSearch(const QString &key);
};
}
#endif // WEBSEARCHLABEL_H
