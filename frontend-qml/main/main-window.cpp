//
// Created by hxf on 22-5-20.
//

#include "main-window.h"
#include "line-edit.h"
#include "result-view.h"
#include "search-worker-manager.h"
#include "icon-item.h"
#include "details-utils.h"
#include "ui-config.h"

//
#include "xatom-helper.h"

#include <QCursor>
#include <QScreen>
#include <QApplication>
#include <QQuickStyle>

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

    MotifWmHints hints;
    hints.flags = MWM_HINTS_FUNCTIONS|MWM_HINTS_DECORATIONS;
    hints.functions = MWM_FUNC_ALL;
    hints.decorations = MWM_DECOR_BORDER;

    XAtomHelper::getInstance()->setWindowMotifHint(m_lineEdit->winId(), hints);
    XAtomHelper::getInstance()->setWindowMotifHint(m_resultView->winId(), hints);
}

void MainWindow::initConnections()
{
    connect(m_lineEdit, &LineEdit::keywordChanged, this, &MainWindow::showResultView);
    connect(qApp, &QApplication::focusWindowChanged, this, &MainWindow::checkFocus);
}

void MainWindow::showWindow()
{
    showLineEdit();
}

void MainWindow::showLineEdit()
{
    if (!m_lineEdit) {
        return;
    }

    if (m_lineEdit->isVisible()) {
        return;
    }

    QScreen *screen = QApplication::screenAt(QCursor::pos());
    int x = screen->geometry().x() + (screen->geometry().width() - m_lineEdit->width()) / 2;
    int y = screen->geometry().y() + screen->geometry().height() / 3;

    m_lineEdit->setPosition(x, y);
    m_lineEdit->show();
}

void MainWindow::showResultView()
{
    if (!m_resultView) {
        return;
    }

    if (m_resultView->isVisible()) {
        return;
    }

    m_resultView->setPosition(m_lineEdit->x(), m_lineEdit->geometry().bottom() + 20);
    m_resultView->show();
}

void MainWindow::hideWindow()
{
    m_lineEdit->hide();
    m_resultView->hide();
}

void MainWindow::initManager()
{
    qmlRegisterType<IconItem>("org.ukui.search.items", 1, 0, "IconItem");
    qmlRegisterType<DetailsUtils>("org.ukui.search.utils", 1, 0, "DetailsUtils");
    qRegisterMetaType<UIConfig*>("UIConfig*");

    QQuickStyle::setStyle("org.kylin.style");

    m_workManager = new SearchWorkerManager(this);
}

void MainWindow::checkFocus(QWindow *focusWindow)
{
    bool lineEditFocus = (focusWindow == m_lineEdit);
    bool resultViewFocus = (focusWindow == m_resultView);

    if (!lineEditFocus && !resultViewFocus) {
        hideWindow();
    }
}
