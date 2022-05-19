#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QQuickView>
#include <QObject>
#include <QScreen>

class MainView : public QQuickView
{
    Q_OBJECT
public:
    MainView();

public Q_SLOTS:
    virtual void onScreenGeometryChanged(QRect);

private:
    QScreen * m_screen = nullptr;
};

#endif // MAINVIEW_H
