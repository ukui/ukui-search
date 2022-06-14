//
// Created by hxf on 22-5-20.
//

#include "main-window.h"
#include "line-edit.h"
#include "result-view.h"
#include "search-worker-manager.h"
#include "icon-item.h"
#include "details-utils.h"

//
#include "xatom-helper.h"

#include <QCursor>
#include <QScreen>
#include <QApplication>
#include <QQuickStyle>

//test
#include <QPushButton>

using namespace UkuiSearch;

MainWindow::MainWindow() : QObject()
{
    initManager();
    initUI();
    initConnections();
}

void MainWindow::initUI()
{
    m_lineEdit = new LineEdit();
    m_lineEdit->installDataProvider(m_workManager);

    m_resultView = new ResultView();
    m_resultView->installDataProvider(m_workManager);

    m_button = new QPushButton("button");
    m_button->resize(100, 50);
    connect(m_button, &QPushButton::clicked, this, &MainWindow::showWindow);
    m_button->show();

    MotifWmHints hints;
    hints.flags = MWM_HINTS_FUNCTIONS|MWM_HINTS_DECORATIONS;
    hints.functions = MWM_FUNC_ALL;
    hints.decorations = MWM_DECOR_BORDER;

    XAtomHelper::getInstance()->setWindowMotifHint(m_lineEdit->winId(), hints);
    XAtomHelper::getInstance()->setWindowMotifHint(m_resultView->winId(), hints);
    XAtomHelper::getInstance()->setWindowMotifHint(m_button->winId(), hints);
}

void MainWindow::initConnections()
{

}

void MainWindow::showWindow()
{
    if (!m_lineEdit) {
        return;
    }

    if (m_lineEdit->isVisible()) {
        m_lineEdit->hide();
        return;
    }

    QScreen *screen = QApplication::screenAt(QCursor::pos());
    int x = screen->geometry().x() + (screen->geometry().width() - m_lineEdit->width()) / 2;
    int y = screen->geometry().y() + screen->geometry().height() / 3;

    qDebug() << "screen:" << screen->geometry() << x << y << m_lineEdit->width();
    qDebug() << "screen:" << screen->geometry() << m_resultView->height() << m_resultView->width();

    m_lineEdit->setPosition(x, y);
    m_resultView->setPosition( x, m_lineEdit->geometry().bottom() + 20);
    m_lineEdit->show();
    m_resultView->show();
}

void MainWindow::initManager()
{
    qmlRegisterType<IconItem>("org.ukui.search.items", 1, 0, "IconItem");
    qmlRegisterType<DetailsUtils>("org.ukui.search.utils", 1, 0, "DetailsUtils");

    QQuickStyle::setStyle("org.kylin.style");

    m_workManager = new SearchWorkerManager(this);
}

