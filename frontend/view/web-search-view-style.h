#ifndef WEBSEARCHVIEWSTYLE_H
#define WEBSEARCHVIEWSTYLE_H

#include <QObject>
#include <QProxyStyle>
namespace Zeeker {
class WebSearchViewStyle : public QProxyStyle
{
    Q_OBJECT
public:
    static WebSearchViewStyle *getStyle();
    void drawControl(QStyle::ControlElement element,
                     const QStyleOption *option,
                     QPainter *painter,
                     const QWidget *widget = nullptr) const;
    void drawPrimitive(PrimitiveElement element,
                       const QStyleOption *option,
                       QPainter *painter,
                       const QWidget *widget = nullptr) const override;
private:
    explicit WebSearchViewStyle(QObject *parent = nullptr);
};
}
#endif // WEBSEARCHVIEWSTYLE_H
