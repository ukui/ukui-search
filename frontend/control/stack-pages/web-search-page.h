#ifndef WEBSEARCHPAGE_H
#define WEBSEARCHPAGE_H

#include <QObject>
#include <QWidget>
namespace Zeeker {
class WebSearchPage : public QWidget
{
    Q_OBJECT
public:
    explicit WebSearchPage(QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *event);

Q_SIGNALS:

};
}

#endif // WEBSEARCHPAGE_H
