/*
 *
 * Copyright (C) 2020, KylinSoft Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: zhangjiaping <zhangjiaping@kylinos.cn>
 * Modified by: zhangpengfei <zhangpengfei@kylinos.cn>
 *
 */

#include "mainwindow.h"
#include <QHBoxLayout>
#include <QDebug>
#include <QDesktopWidget>
#include <syslog.h>
#include <QPalette>
#include <QScreen>
#include <QStyleOption>
#include <QPixmap>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
#include <KWindowEffects>
#include "kwindowsystem.h"
#endif
#include "global-settings.h"
#include <QtX11Extras/QX11Info>

#define MAIN_MARGINS 0, 0, 0, 0
#define TITLE_MARGINS 0,0,0,0
#define UKUI_SEARCH_SCHEMAS "org.ukui.search.settings"
#define SEARCH_METHOD_KEY "indexSearch"
#define WEB_ENGINE_KEY "webEngine"
#define WINDOW_WIDTH 700
#define WINDOW_HEIGHT 610
#define TITLE_HEIGHT 40
#define WINDOW_ICON_SIZE 24
#define SETTING_BTN_SIZE 30
#define SEARCH_BAR_SIZE 48
#define ASK_INDEX_TIME 5*1000
#define RESEARCH_TIME 10*1000

using namespace UkuiSearch;
extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);
/**
 * @brief MainWindow 主界面
 * @param parent
 *
 * 慎用KWindowSystem::setShowingDesktop(!KWindowSystem::showingDesktop());
 * 可能造成窗口属性的混乱
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent) {
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setAutoFillBackground(false);
    this->setFocusPolicy(Qt::StrongFocus);
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    this->setWindowTitle(tr("ukui-search"));
    initUi();
    initTimer();

    m_sys_tray_icon = new QSystemTrayIcon(this);
    m_sys_tray_icon->setIcon(QIcon::fromTheme("system-search-symbolic"));
    m_sys_tray_icon->setToolTip(tr("Global Search"));
    m_sys_tray_icon->show();
    installEventFilter(this);
    initConnections();
    initGsettings();

    connect(KWindowSystem::self(), &KWindowSystem::activeWindowChanged, this,[&](WId activeWindowId){
            if (activeWindowId != this->winId()) {
                tryHideMainwindow();
            }
        });

    //NEW_TODO, register plugins
//    SearchPluginManager::getInstance()->registerPlugin(\\);
//    m_stackedWidget->setPlugins(SearchPluginManager::getInstance()->getPluginIds());
//    m_stackedWidget->setPlugins(SearchPluginManager::getInstance()->getPluginIds());
}

MainWindow::~MainWindow() {
#if (QT_VERSION < QT_VERSION_CHECK(5, 12, 0))
    if(m_settingsWidget) {
        delete m_settingsWidget;
        m_settingsWidget = NULL;

#endif
    if(m_askDialog) {
        delete m_askDialog;
        m_askDialog = NULL;
    }
    if(m_askTimer) {
        delete m_askTimer;
        m_askTimer = NULL;
    }
    if(m_search_gsettings) {
        delete m_search_gsettings;
        m_search_gsettings = NULL;
    }
}

/**
 * @brief initUi 初始化主界面主要ui控件
 */
void MainWindow::initUi() {
    this->setFixedSize(WINDOW_WIDTH, 68);
//    this->setStyleSheet("QMainWindow{border:2px solid red;}");

//    m_widget = new QWidget(this);

//    this->setCentralWidget(m_widget);
//    m_widget->setFixedSize(this->size());
//    QVBoxLayout * mainlayout = new QVBoxLayout(m_frame);
//    mainlayout->setContentsMargins(MAIN_MARGINS);
//    m_frame->setLayout(mainlayout);

//    m_stackedWidget = new StackedWidget(m_frame);//内容栏
    m_searchBarWidget = new SeachBarWidget(this);
    m_searchBarWidget->move(this->rect().topLeft());
    m_searchBarWidget->show();
    m_searchResultPage = new SearchResultPage(this);
    m_searchResultPage->hide();
    m_searchResultPage->move(0, 58);
    this->setFocusProxy(m_searchBarWidget);

//    m_searchResultPage->show();
//    m_searchWidget = new SeachBarWidget(this);
//    m_searchLayout = new SearchBarHLayout(this);
//    m_searchWidget->setLayout(m_searchLayout);
//    m_searchWidget->setFixedHeight(SEARCH_BAR_SIZE);

//    mainlayout->addWidget(m_titleFrame);
//    mainlayout->addWidget(m_seachBarWidget);
//    mainlayout->addSpacing(8);
//    mainlayout->addWidget(m_searchResultPage);

    //创建索引询问弹窗
    m_askDialog = new CreateIndexAskDialog(this);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
    MotifWmHints ask_dialog_hints;
    ask_dialog_hints.flags = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
    ask_dialog_hints.functions = MWM_FUNC_ALL;
    ask_dialog_hints.decorations = MWM_DECOR_BORDER;
    XAtomHelper::getInstance()->setWindowMotifHint(m_askDialog->winId(), ask_dialog_hints);
#endif
}

void MainWindow::initConnections()
{
    connect(m_sys_tray_icon, &QSystemTrayIcon::activated, this, &MainWindow::trayIconActivatedSlot);
    QObject::connect(this, &MainWindow::searchMethodChanged, this, [ = ](FileUtils::SearchMethod sm) {
        FileUtils::searchMethod = sm;
    });
    connect(QApplication::primaryScreen(), &QScreen::geometryChanged, this, &MainWindow::monitorResolutionChange);
    connect(qApp, &QApplication::primaryScreenChanged, this, &MainWindow::primaryScreenChangedSlot);
    connect(m_askDialog, &CreateIndexAskDialog::closed, this, [ = ]() {
        m_isAskDialogVisible = false;
    });
    connect(m_askDialog, &CreateIndexAskDialog::btnClicked, this, [ = ](const bool &is_create_index, const bool &is_ask_again) {
        setSearchMethodConfig(is_create_index, is_ask_again);
    });
    connect(m_settingsBtn, &QPushButton::clicked, this, &MainWindow::settingsBtnClickedSlot);
    //主题改变时，更新自定义标题栏的图标
//    connect(qApp, &QApplication::paletteChanged, this, [ = ]() {
//        m_iconLabel->setPixmap(QIcon::fromTheme("kylin-search").pixmap(QSize(WINDOW_ICON_SIZE, WINDOW_ICON_SIZE)));
//    });
    connect(m_searchBarWidget, &SeachBarWidget::requestSearchKeyword, this, &MainWindow::searchKeywordSlot);
//    connect(m_stackedWidget, &StackedWidget::effectiveSearch, m_searchLayout, &SearchBarHLayout::effectiveSearchRecord);
    //connect(m_searchResultPage, &SearchResultPage::resizeHeight, this, &MainWindow::resizeHeight);
    connect(this,&MainWindow::setText,m_searchBarWidget,&SeachBarWidget::setText);
}

/**
 * @brief bootOptionsFilter 过滤终端命令
 * @param opt
 */
void MainWindow::bootOptionsFilter(QString opt) {
    if(opt == "-s" || opt == "--show") {
        clearSearchResult();
        centerToScreen(this);
        if(this->isHidden()) {
            this->show();
        }
        this->m_searchBarWidget->setFocus();
        this->activateWindow();
    }
}

/**
 * @brief clearSearchResult 清空搜索结果
 */
void MainWindow::clearSearchResult() {
    m_searchBarWidget->clear();
//    m_searchLineEdit->clearFocus();
}

/**
 * @brief MainWindow::trayIconActivatedSlot 点击任务栏图标的槽函数
 * @param reason
 */
void MainWindow::trayIconActivatedSlot(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::Trigger) {
        if(!this->isVisible()) {
            clearSearchResult();
            centerToScreen(this);
            this->show();
//            this->m_searchLineEdit->focusIn(); //打开主界面时输入框夺焦，可直接输入
            this->raise();
            this->activateWindow();
        } else {
            tryHideMainwindow();
        }
    }
}

/**
 * @brief setSearchMethodConfig 在询问弹窗点击按钮后执行
 * @param create_index 是否同意创建索引
 * @param no_longer_ask 是否勾选了不再提示
 */
void MainWindow::setSearchMethodConfig(const bool &create_index, const bool &no_longer_ask)
{
    if(no_longer_ask) {
        GlobalSettings::getInstance()->setValue(ENABLE_CREATE_INDEX_ASK_DIALOG, "false");
    } else {
        GlobalSettings::getInstance()->setValue(ENABLE_CREATE_INDEX_ASK_DIALOG, "true");
    }
    if(create_index) {
        if(m_search_gsettings && m_search_gsettings->keys().contains(SEARCH_METHOD_KEY)) {
            m_search_gsettings->set(SEARCH_METHOD_KEY, true);
        }
        Q_EMIT this->searchMethodChanged(FileUtils::SearchMethod::INDEXSEARCH);
        //创建索引十秒后重新搜索一次(如果用户十秒内没有退出搜索界面且没有重新搜索)
        m_researchTimer->start();
    }
}

/**
 * @brief MainWindow::settingsBtnClickedSlot 点击设置按钮的槽函数
 */
void MainWindow::settingsBtnClickedSlot()
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 12, 0))
    if(m_settingsWidget) {  //当此窗口已存在时，仅需置顶
        if(!m_settingsWidget->isVisible()) {
            centerToScreen(m_settingsWidget);
        }
        m_settingsWidget->showWidget();
        return;
    }
    m_settingsWidget = new SettingsWidget();
    connect(this, &MainWindow::webEngineChanged, m_settingsWidget, [ = ]() {
        m_settingsWidget->resetWebEngine();
    });
    connect(m_settingsWidget, &SettingsWidget::webEngineChanged, this, [ = ](const QString & engine) {
        if(m_search_gsettings && m_search_gsettings->keys().contains(WEB_ENGINE_KEY)) {
            m_search_gsettings->set(WEB_ENGINE_KEY, engine);
        } else {
            GlobalSettings::getInstance()->setValue(WEB_ENGINE, engine);
        }
    });
    centerToScreen(m_settingsWidget);
    m_settingsWidget->show();
    connect(m_settingsWidget, &SettingsWidget::settingWidgetClosed, this, [ = ]() {
        QTimer::singleShot(100, this, [ = ] {
//                clearSearchResult(); //现暂定从设置页返回主页面不清空搜索结果
            this->setWindowState(this->windowState() & ~Qt::WindowMinimized);
            this->raise();
            this->showNormal();
            this->activateWindow();
        });
    });
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
    //打开控制面板的设置页
    QProcess process;
    process.startDetached("ukui-control-center -m search");
#endif
}

/**
 * @brief MainWindow::searchKeywordSlot 执行搜索的槽函数
 * @param keyword 关键词
 */
void MainWindow::searchKeywordSlot(const QString &keyword)
{
    //NEW_TODO
    if(keyword == "") {
//        m_stackedWidget->setPage(int(StackedPage::HomePage));
        m_askTimer->stop();
        Q_EMIT m_searchResultPage->stopSearch();
        m_searchResultPage->hide();
        this->resizeHeight(68);

    } else {
//        m_stackedWidget->setPage(int(StackedPage::SearchPage));
        QTimer::singleShot(10, this, [ = ]() {
            //允许弹窗且当前次搜索（为关闭主界面，算一次搜索过程）未询问且当前为暴力搜索
            if(GlobalSettings::getInstance()->getValue(ENABLE_CREATE_INDEX_ASK_DIALOG).toString() != "false" && !m_currentSearchAsked && FileUtils::searchMethod == FileUtils::SearchMethod::DIRECTSEARCH)
                m_askTimer->start();
            Q_EMIT m_searchResultPage->startSearch(keyword);
            this->resizeHeight(WINDOW_HEIGHT);

            m_searchResultPage->move(0, 58);
            m_searchResultPage->show();
        });
    }
    m_researchTimer->stop(); //如果搜索内容发生改变，则停止建索引后重新搜索的倒计时
}

void MainWindow::resizeHeight(int height)
{
    this->setFixedHeight(height);
}

/**
 * @brief monitorResolutionChange  监听屏幕改变
 * @param rect
 */
void MainWindow::monitorResolutionChange(QRect rect) {
    Q_UNUSED(rect);
}

/**
 * @brief primaryScreenChangedSlot 监听分辨率改变
 * @param screen
 */
void MainWindow::primaryScreenChangedSlot(QScreen *screen) {
    Q_UNUSED(screen);

}

/**
 * @brief MainWindow::moveToPanel 将主界面移动到任务栏旁边（跟随任务栏位置）
 */
void MainWindow::moveToPanel() {
    QRect availableGeometry = qApp->primaryScreen()->availableGeometry();
    QRect screenGeometry = qApp->primaryScreen()->geometry();

    QDBusInterface primaryScreenInterface("org.ukui.SettingsDaemon",
                                          "/org/ukui/SettingsDaemon/wayland",
                                          "org.ukui.SettingsDaemon.wayland",
                                          QDBusConnection::sessionBus());
    if(QDBusReply<int>(primaryScreenInterface.call("x")).isValid()) {
        QDBusReply<int> x = primaryScreenInterface.call("x");
        QDBusReply<int> y = primaryScreenInterface.call("y");
        QDBusReply<int> width = primaryScreenInterface.call("width");
        QDBusReply<int> height = primaryScreenInterface.call("height");
        screenGeometry.setX(x);
        screenGeometry.setY(y);
        screenGeometry.setWidth(width);
        screenGeometry.setHeight(height);
        availableGeometry.setX(x);
        availableGeometry.setY(y);
        availableGeometry.setWidth(width);
        availableGeometry.setHeight(height);
    }

    QDesktopWidget * desktopWidget = QApplication::desktop();
    QRect screenMainRect = desktopWidget->screenGeometry(0);//获取设备屏幕大小

    QDBusInterface interface("com.ukui.panel.desktop",
                                 "/",
                                 "com.ukui.panel.desktop",
                                 QDBusConnection::sessionBus());

    int position = QDBusReply<int>(interface.call("GetPanelPosition", "position"));
    int height = QDBusReply<int>(interface.call("GetPanelSize", "height"));
    int d = 8; //窗口边沿到任务栏距离

    if(position == 0) {
        //任务栏在下侧
        this->move(availableGeometry.x() + availableGeometry.width() - this->width() - d, screenGeometry.y() + screenGeometry.height() - this->height() - height - d);
    } else if(position == 1) {
        //任务栏在上侧
        this->move(availableGeometry.x() + availableGeometry.width() - this->width() - d, screenGeometry.y() + height + d);
    } else if(position == 2) {
        //任务栏在左侧
        this->move(screenGeometry.x() + height + d, screenGeometry.y() + screenGeometry.height() - this->height() - d);
    } else if(position == 3) {
        //任务栏在右侧
        this->move(screenGeometry.x() + screenGeometry.width() - this->width() - height - d, screenGeometry.y() + screenGeometry.height() - this->height() - d);
    }
}

/**
 * @brief MainWindow::centerToScreen 使窗口显示在屏幕中间
 * @param widget
 */
void MainWindow::centerToScreen(QWidget* widget) {
    if(!widget)
        return;
    QDesktopWidget* m = QApplication::desktop();
    QRect desk_rect = m->screenGeometry(m->screenNumber(QCursor::pos()));
    int desk_x = desk_rect.width();
    int desk_y = desk_rect.height();
    int x = widget->width();
    int y = widget->height();
//    QDBusInterface primaryScreenInterface("org.ukui.SettingsDaemon",
//                                          "/org/ukui/SettingsDaemon/wayland",
//                                          "org.ukui.SettingsDaemon.wayland",
//                                          QDBusConnection::sessionBus());
//    if(QDBusReply<int>(primaryScreenInterface.call("x")).isValid()) {
//        QDBusReply<int> width = primaryScreenInterface.call("width");
//        QDBusReply<int> height = primaryScreenInterface.call("height");
//        desk_x = width;
//        desk_y = height;
//    }
    widget->move(desk_x / 2 - x / 2 + desk_rect.left(), desk_y / 3 + desk_rect.top());
}

void MainWindow::initGsettings() {
    const QByteArray id(UKUI_SEARCH_SCHEMAS);
    if(QGSettings::isSchemaInstalled(id)) {
        m_search_gsettings = new QGSettings(id);
        connect(m_search_gsettings, &QGSettings::changed, this, [ = ](const QString & key) {
            if(key == SEARCH_METHOD_KEY) {
                bool is_index_search = m_search_gsettings->get(SEARCH_METHOD_KEY).toBool();
                this->setSearchMethod(is_index_search);
            } else if(key == WEB_ENGINE_KEY) {
                QString web_engine = m_search_gsettings->get(WEB_ENGINE_KEY).toString();
                GlobalSettings::getInstance()->setValue(WEB_ENGINE, web_engine);
                Q_EMIT this->webEngineChanged();
            }
        });
        if(m_search_gsettings->keys().contains(SEARCH_METHOD_KEY)) {
            bool is_index_search = m_search_gsettings->get(SEARCH_METHOD_KEY).toBool();
            this->setSearchMethod(is_index_search);
        }
        if(m_search_gsettings->keys().contains(WEB_ENGINE_KEY)) {
            QString web_engine = m_search_gsettings->get(WEB_ENGINE_KEY).toString();
            GlobalSettings::getInstance()->setValue(WEB_ENGINE, web_engine);
        }
    }
}

//使用GSetting获取当前窗口应该使用的透明度
double MainWindow::getTransparentData() {
    return GlobalSettings::getInstance()->getValue(TRANSPARENCY_KEY).toDouble();
}

void MainWindow::initTimer() {
    m_askTimer = new QTimer;
    m_askTimer->setInterval(ASK_INDEX_TIME);
    connect(m_askTimer, &QTimer::timeout, this, [ = ]() {
        if(this->isVisible()) {
            m_isAskDialogVisible = true;
            m_askDialog->show();
            m_currentSearchAsked = true;
        }
        m_askTimer->stop();
    });
    m_researchTimer = new QTimer;
    m_researchTimer->setInterval(RESEARCH_TIME);
    connect(m_researchTimer, &QTimer::timeout, this, [ = ]() {
        if(this->isVisible()) {
            m_searchBarWidget->reSearch();
        }
        m_researchTimer->stop();
    });
    connect(m_searchBarWidget, &SeachBarWidget::requestSearchKeyword, this, [ = ](QString text) {
        if(text == "" || text.isEmpty()) {
            m_askTimer->stop();
        } else {
            //允许弹窗且当前次搜索（为关闭主界面，算一次搜索过程）未询问且当前为暴力搜索
            if(GlobalSettings::getInstance()->getValue(ENABLE_CREATE_INDEX_ASK_DIALOG).toString() != "false" && !m_currentSearchAsked && FileUtils::searchMethod == FileUtils::SearchMethod::DIRECTSEARCH)
                m_askTimer->start();
        }
    });
}

/**
 * @brief MainWindow::tryHideMainwindow 尝试隐藏主界面并停止部分未完成的动作，重置部分状态值
 */
bool MainWindow::tryHideMainwindow()
{
    if (!m_isAskDialogVisible && QApplication::activeModalWidget() == nullptr) {
        qDebug()<<"Mainwindow will be hidden";
        m_currentSearchAsked = false;
        this->hide();
        m_askTimer->stop();
        m_researchTimer->stop();
        Q_EMIT m_searchResultPage->stopSearch();
        return true;
    } else {
        //有上层弹窗未关闭，不允许隐藏主界面
        qWarning()<<"There is a dialog onside, so that mainwindow can not be hidden.";
        return false;
    }
}

/**
 * @brief MainWindow::setSearchMethod 设置搜索模式
 * @param is_index_search true为索引搜索，false为暴力搜索
 */
void MainWindow::setSearchMethod(const bool &is_index_search) {
    if(is_index_search) {
        Q_EMIT this->searchMethodChanged(FileUtils::SearchMethod::INDEXSEARCH);
    } else {
        Q_EMIT this->searchMethodChanged(FileUtils::SearchMethod::DIRECTSEARCH);
        m_researchTimer->stop();
    }
}

/**
 * @brief MainWindow::keyPressEvent 按esc键关闭主界面
 * @param event
 */
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        tryHideMainwindow();
    } else if (event->key() == Qt::Key_Return or event->key() == Qt::Key_Enter) {
        if (!m_searchResultPage->isHidden()) {
            //显示最佳匹配中第一项的详情页，无搜索结果则调取网页搜索
            qDebug() << "Press Enter";
            m_searchResultPage->pressEnter();
        }
    } else if (event->key() == Qt::Key_Up) {
        qDebug() << "Press ↑";
        m_searchResultPage->pressUp();
    } else if (event->key() == Qt::Key_Down) {
        qDebug() << "Press ↓";
        if (!m_searchResultPage->getSelectedState()) {
            m_searchResultPage->pressEnter();
        } else {
            m_searchResultPage->pressDown();
        }
    }
    return QWidget::keyPressEvent(event);
}

void MainWindow::paintEvent(QPaintEvent *event) {

    QPainterPath path;

    path.addRoundedRect(m_searchBarWidget->x()+10, m_searchBarWidget->y()+10, m_searchBarWidget->width()-20, m_searchBarWidget->height()-20, 6, 6);
    path.addRoundedRect(m_searchResultPage->x()+10, m_searchResultPage->y()+10, m_searchResultPage->width()-20, m_searchResultPage->height()-20, 6, 6);

    KWindowEffects::enableBlurBehind(this->winId(), true, QRegion(path.toFillPolygon().toPolygon()));

}
