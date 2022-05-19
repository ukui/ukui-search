#include "main-view.h"
#include <KWindowSystem>
#include <QGuiApplication>
#include <QCursor>
#include <KWindowEffects>


MainView::MainView() : QQuickView()
{
    setResizeMode(QQuickView::SizeViewToRootObject);
    setColor(Qt::transparent);
    m_screen = QGuiApplication::screenAt(QCursor::pos());
    connect(m_screen, &QScreen::geometryChanged, this, &MainView::onScreenGeometryChanged);
    KWindowEffects::enableBlurBehind(this, true, QRegion());
}

void MainView::onScreenGeometryChanged(QRect)
{

}
