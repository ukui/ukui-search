#include "search-line-edit-view.h"
#include <QApplication>
#include <QScreen>
#include <KWindowEffects>
#include <KWindowSystem>
//#include <KWindowSystem>

SearchLineEditView::SearchLineEditView() : QQuickView()
{
    setResizeMode(QQuickView::SizeViewToRootObject);
    setColor(Qt::transparent);
    KWindowEffects::enableBlurBehind(this, true, QRegion());
    m_desktopWidget = QApplication::desktop();
    connect(m_desktopWidget, &QDesktopWidget::resized, this, &SearchLineEditView::showWindow);
    KWindowSystem::setState(this->winId(),NET::SkipTaskbar | NET::SkipPager | NET::SkipSwitcher );
    this->setSource(QUrl(QStringLiteral("qrc:/SearchLineEdit.qml")));

    QObject::connect(reinterpret_cast<QObject*>(this->rootObject()), SIGNAL(textChange(QString)), this, SLOT(onTextChange(QString)));

}

void SearchLineEditView::showWindow()
{
    QScreen *m_screen = QApplication::screenAt(QCursor::pos());
    setPosition(m_screen->geometry().width()/ 2 - this->width() / 2 + m_screen->geometry().x(),
                m_screen->geometry().height() / 3 + m_screen->geometry().top());
    this->show();
}

void SearchLineEditView::onTextChange(QString text)
{
    Q_EMIT textChange(text);
}
