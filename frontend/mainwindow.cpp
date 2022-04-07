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
#include "qt-single-application.h"
#include "global-settings.h"
#include <QtX11Extras/QX11Info>

#define MAIN_MARGINS 0, 0, 0, 0
#define TITLE_MARGINS 0,0,0,0
#define UKUI_SEARCH_SCHEMAS "org.ukui.search.settings"
#define BACKGROUND_SCHEMAS  "org.mate.background"
#define PICTURE_FILE_NAME   "pictureFilename"
#define PRIMARY_COLOR       "primaryColor"
#define PICTURE_OPTIONS     "pictureOptions"
#define PICTURE_BLUR_RADIUS  25
#define SEARCH_METHOD_KEY "indexSearch"
#define WEB_ENGINE_KEY "webEngine"
#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 808
#define WINDOW_SPACE 8
#define TITLE_HEIGHT 40
#define WINDOW_ICON_SIZE 24
#define SETTING_BTN_SIZE 30
#define SEARCH_BAR_SIZE 48
#define ASK_INDEX_TIME 5*1000
#define RESEARCH_TIME 10*1000

using namespace Zeeker;
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
    this->setWindowState(Qt::WindowFullScreen);

//    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    this->setWindowTitle(tr("ukui-search"));
    KWindowSystem::setState(this->winId(),NET::SkipTaskbar | NET::SkipPager | NET::SkipSwitcher );
    initUi();
//    initTimer();

    m_sys_tray_icon = new QSystemTrayIcon(this);
    m_sys_tray_icon->setIcon(QIcon::fromTheme("preferences-system-search-symbolic"));
    m_sys_tray_icon->setToolTip(tr("Global Search"));
    m_sys_tray_icon->show();
    installEventFilter(this);
    initConnections();
    initGsettings();
    //NEW_TODO, register plugins
//    SearchPluginManager::getInstance()->registerPlugin(\\);
//    m_stackedWidget->setPlugins(SearchPluginManager::getInstance()->getPluginIds());
//    m_stackedWidget->setPlugins(SearchPluginManager::getInstance()->getPluginIds());
}

MainWindow::~MainWindow() {
#if (QT_VERSION < QT_VERSION_CHECK(5, 12, 0))
    if (m_settingsWidget) {
        delete m_settingsWidget;
        m_settingsWidget = NULL;
    }
#endif
//    if (m_askDialog) {
//        delete m_askDialog;
//        m_askDialog = NULL;
//    }
//    if (m_askTimer) {
//        delete m_askTimer;
//        m_askTimer = NULL;
//    }
    if (m_search_gsettings) {
        delete m_search_gsettings;
        m_search_gsettings = NULL;
    }
}

/**
 * @brief initUi 初始化主界面主要ui控件
 */
void MainWindow::initUi() {
    this->setGeometry(QGuiApplication::screenAt(QCursor::pos())->geometry());
    m_desktopWidget = QApplication::desktop();
    connect(m_desktopWidget, &QDesktopWidget::resized, this, &MainWindow::centerToScreen);
//    this->setStyleSheet("QMainWindow{border:2px solid red;}");
    m_widget = new QWidget(this);
    m_widget->setFixedSize(WINDOW_WIDTH, SEARCH_BAR_SIZE);
//    this->setCentralWidget(m_widget);
    m_mainLayout = new QVBoxLayout(m_widget);
    m_mainLayout->setContentsMargins(MAIN_MARGINS);
    m_mainLayout->setSpacing(WINDOW_SPACE);
//    m_frame->setLayout(mainlayout);

//    m_stackedWidget = new StackedWidget(m_frame);//内容栏
    m_searchBarWidget = new SeachBarWidget(this);
    m_mainLayout->addWidget(m_searchBarWidget);
    m_searchBarWidget->show();
    m_searchResultPage = new SearchResultPage(this);
    m_mainLayout->addWidget(m_searchResultPage);
    m_searchResultPage->hide();
//    m_searchResultPage->move(0, 58);
    m_webSearchPage = new WebSearchPage(this);
    m_mainLayout->addWidget(m_webSearchPage);
    m_webSearchPage->hide();
    this->setFocusProxy(m_searchBarWidget);
    m_backgroundImage = QImage(":/res/icons/wallpaper.png");

}

void MainWindow::initConnections()
{
    connect(m_sys_tray_icon, &QSystemTrayIcon::activated, this, &MainWindow::trayIconActivatedSlot);
    QObject::connect(this, &MainWindow::searchMethodChanged, this, [ = ](FileUtils::SearchMethod sm) {
        this->m_searchMethodManager.searchMethod(sm);
    });
    connect(QApplication::primaryScreen(), &QScreen::geometryChanged, this, &MainWindow::monitorResolutionChange);
    connect(qApp, &QApplication::primaryScreenChanged, this, &MainWindow::primaryScreenChangedSlot);
    connect(m_settingsBtn, &QPushButton::clicked, this, &MainWindow::settingsBtnClickedSlot);
    //主题改变时，更新自定义标题栏的图标
//    connect(qApp, &QApplication::paletteChanged, this, [ = ]() {
//        m_iconLabel->setPixmap(QIcon::fromTheme("kylin-search").pixmap(QSize(WINDOW_ICON_SIZE, WINDOW_ICON_SIZE)));
//    });
    connect(m_searchBarWidget, &SeachBarWidget::requestSearchKeyword, this, &MainWindow::searchKeywordSlot);
    connect(this,&MainWindow::setText,m_searchBarWidget,&SeachBarWidget::setText);
    //connect(m_searchResultPage, &SearchResultPage::setWebSearchSelection, m_webSearchPage, &WebSearchPage::setWebSearchSelection);
    connect(this, &MainWindow::startWebSearch, m_webSearchPage, &WebSearchPage::startSearch);
    connect(m_searchResultPage, &SearchResultPage::getResult, this, [=] () {
        this->resizeHeight(WINDOW_HEIGHT);
        m_searchResultPage->show();
        m_webSearchPage->clearResultSelection();
    });
}

/**
 * @brief bootOptionsFilter 过滤终端命令
 * @param opt
 */
void MainWindow::bootOptionsFilter(QString opt) {
    if (opt == "-s" || opt == "--show") {
        clearSearchResult();
        centerToScreen();
        if (this->isHidden()) {
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
    if (reason == QSystemTrayIcon::Trigger) {
        if (!this->isVisible()) {
            bootOptionsFilter("-s");
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
    if (no_longer_ask) {
        GlobalSettings::getInstance()->setValue(ENABLE_CREATE_INDEX_ASK_DIALOG, "false");
    } else {
        GlobalSettings::getInstance()->setValue(ENABLE_CREATE_INDEX_ASK_DIALOG, "true");
    }
    if (create_index) {
        if (m_search_gsettings && m_search_gsettings->keys().contains(SEARCH_METHOD_KEY)) {
            m_search_gsettings->set(SEARCH_METHOD_KEY, true);
        } else {
            //调用创建索引接口
            Q_EMIT this->searchMethodChanged(FileUtils::SearchMethod::INDEXSEARCH);
            //创建索引十秒后重新搜索一次(如果用户十秒内没有退出搜索界面且没有重新搜索)
            m_researchTimer->start();
        }
    }
}

/**
 * @brief MainWindow::settingsBtnClickedSlot 点击设置按钮的槽函数
 */
void MainWindow::settingsBtnClickedSlot()
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 12, 0))
    if (m_settingsWidget) {  //当此窗口已存在时，仅需置顶
        if (!m_settingsWidget->isVisible()) {
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
        if (m_search_gsettings && m_search_gsettings->keys().contains(WEB_ENGINE_KEY)) {
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
    process.startDetached("ukui-control-center --search");
#endif
}

/**
 * @brief MainWindow::searchKeywordSlot 执行搜索的槽函数
 * @param keyword 关键词
 */
void MainWindow::searchKeywordSlot(const QString &keyword)
{
    //NEW_TODO
    if (keyword == "") {
        Q_EMIT m_searchResultPage->stopSearch();
        m_searchResultPage->hide();
        m_webSearchPage->hide();
        this->resizeHeight(SEARCH_BAR_SIZE);
    } else {
        QTimer::singleShot(10, this, [ = ]() {
            Q_EMIT m_searchResultPage->startSearch(keyword);
            this->resizeHeight(SEARCH_BAR_SIZE + WINDOW_SPACE + m_webSearchPage->height());
            Q_EMIT this->startWebSearch(keyword);
            m_searchResultPage->hide();
            m_webSearchPage->show();
        });
    }
}

void MainWindow::resizeHeight(int height)
{
//    this->setFixedHeight(height);
    m_widget->setFixedHeight(height);
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
    if (QDBusReply<int>(primaryScreenInterface.call("x")).isValid()) {
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
    int d = WINDOW_SPACE; //窗口边沿到任务栏距离

    if (position == 0) {
        //任务栏在下侧
        this->move(availableGeometry.x() + availableGeometry.width() - this->width() - d, screenGeometry.y() + screenGeometry.height() - this->height() - height - d);
    } else if (position == 1) {
        //任务栏在上侧
        this->move(availableGeometry.x() + availableGeometry.width() - this->width() - d, screenGeometry.y() + height + d);
    } else if (position == 2) {
        //任务栏在左侧
        this->move(screenGeometry.x() + height + d, screenGeometry.y() + screenGeometry.height() - this->height() - d);
    } else if (position == 3) {
        //任务栏在右侧
        this->move(screenGeometry.x() + screenGeometry.width() - this->width() - height - d, screenGeometry.y() + screenGeometry.height() - this->height() - d);
    }
}

/**
 * @brief MainWindow::centerToScreen 使窗口显示在屏幕中间
 * @param widget
 */
void MainWindow::centerToScreen() {
    KWindowSystem::setState(this->winId(),NET::SkipTaskbar | NET::SkipPager);
    this->setWindowState(Qt::WindowFullScreen);
    this->setGeometry(QGuiApplication::screenAt(QCursor::pos())->geometry());
    m_widget->move((this->width() - WINDOW_WIDTH) / 2, this->height() / 8);
}

void MainWindow::initGsettings() {
    const QByteArray id(UKUI_SEARCH_SCHEMAS);
    if (QGSettings::isSchemaInstalled(id)) {
        m_search_gsettings = new QGSettings(id);
        connect(m_search_gsettings, &QGSettings::changed, this, [ = ](const QString & key) {
            if (key == SEARCH_METHOD_KEY) {
                bool is_index_search = m_search_gsettings->get(SEARCH_METHOD_KEY).toBool();
                this->setSearchMethod(is_index_search);
            } else if (key == WEB_ENGINE_KEY) {
                QString web_engine = m_search_gsettings->get(WEB_ENGINE_KEY).toString();
                GlobalSettings::getInstance()->setValue(WEB_ENGINE, web_engine);
                Q_EMIT this->webEngineChanged();
            }
        });
        if (m_search_gsettings->keys().contains(SEARCH_METHOD_KEY)) {
            bool is_index_search = m_search_gsettings->get(SEARCH_METHOD_KEY).toBool();
            this->setSearchMethod(is_index_search);
        }
        if (m_search_gsettings->keys().contains(WEB_ENGINE_KEY)) {
            QString web_engine = m_search_gsettings->get(WEB_ENGINE_KEY).toString();
            GlobalSettings::getInstance()->setValue(WEB_ENGINE, web_engine);
        }
    }

    if (QGSettings::isSchemaInstalled(BACKGROUND_SCHEMAS)) {
        m_backgroundGSetting = new QGSettings(BACKGROUND_SCHEMAS, QByteArray(), this);

        QStringList keys = m_backgroundGSetting->keys();
        if (keys.contains(PICTURE_FILE_NAME) && keys.contains(PICTURE_OPTIONS) && keys.contains(PRIMARY_COLOR)) {
            //不需要监听

        } else {
            qt_blurImage(m_backgroundImage, PICTURE_BLUR_RADIUS, false, 0);
            m_backgroundGSetting->deleteLater();
            m_backgroundGSetting = nullptr;
        }
    }
}

void MainWindow::hideMainWindow()
{
    tryHideMainwindow();
}

//使用GSetting获取当前窗口应该使用的透明度
double MainWindow::getTransparentData() {
    return GlobalSettings::getInstance()->getValue(TRANSPARENCY_KEY).toDouble();
}

void MainWindow::initTimer() {
    m_askTimer = new QTimer;
    m_askTimer->setInterval(ASK_INDEX_TIME);
    connect(m_askTimer, &QTimer::timeout, this, [ = ]() {
        if (this->isVisible()) {
//            m_isAskDialogVisible = true;
//            m_askDialog->show();
            m_currentSearchAsked = true;
        }
        m_askTimer->stop();
    });
    m_researchTimer = new QTimer;
    m_researchTimer->setInterval(RESEARCH_TIME);
    connect(m_researchTimer, &QTimer::timeout, this, [ = ]() {
        if (this->isVisible()) {
            m_searchBarWidget->reSearch();
        }
        m_researchTimer->stop();
    });
    connect(m_searchBarWidget, &SeachBarWidget::requestSearchKeyword, this, [ = ](QString text) {
        if (text == "" || text.isEmpty()) {
            m_askTimer->stop();
        } else {
            //允许弹窗且当前次搜索（为关闭主界面，算一次搜索过程）未询问且当前为暴力搜索
            if (GlobalSettings::getInstance()->getValue(ENABLE_CREATE_INDEX_ASK_DIALOG).toString() != "false" && !m_currentSearchAsked && FileUtils::searchMethod == FileUtils::SearchMethod::DIRECTSEARCH)
                m_askTimer->start();
        }
    });
}

/**
 * @brief MainWindow::tryHideMainwindow 尝试隐藏主界面并停止部分未完成的动作，重置部分状态值
 */
bool MainWindow::tryHideMainwindow()
{
    if (QApplication::activeModalWidget() == nullptr) {
        qDebug()<<"Mainwindow will be hidden";
//        m_currentSearchAsked = false;
        this->hide();
//        m_askTimer->stop();
//        m_researchTimer->stop();
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
    if (is_index_search) {
        //调用创建索引接口
        Q_EMIT this->searchMethodChanged(FileUtils::SearchMethod::INDEXSEARCH);
        //创建索引十秒后重新搜索一次(如果用户十秒内没有退出搜索界面且没有重新搜索)
//        m_researchTimer->start();
    } else {
        Q_EMIT this->searchMethodChanged(FileUtils::SearchMethod::DIRECTSEARCH);
//        m_researchTimer->stop();
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
        if (!m_webSearchPage->isHidden()) {//屏蔽特殊情况
            //显示最佳匹配中第一项的详情页，无搜索结果则调取网页搜索
            qDebug() << "Press Enter";
            if (m_searchResultPage->isHidden()) {
                m_webSearchPage->LaunchBrowser();
            } else {
                m_searchResultPage->pressEnter();
            }
        }
    } else if (event->key() == Qt::Key_Up) {
        qDebug() << "Press ↑";
        //web选中状态暂无操作
        if (!m_searchResultPage->isHidden()) {
            m_searchResultPage->pressUp();
        }
    } else if (event->key() == Qt::Key_Down) {
        qDebug() << "Press ↓";
        if (!m_searchResultPage->isHidden()) {
            m_searchResultPage->pressDown();
        }
    }
    return QWidget::keyPressEvent(event);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::ActivationChange) {
        qDebug() << "QEvent::ActivationChange!!!!" << "active" << (QApplication::activeWindow() == this) << "isVisble" << (this->isVisible());
        if (QApplication::activeWindow() != this) {
            tryHideMainwindow();
        }
    }
    return QMainWindow::eventFilter(watched,event);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(!m_widget->rect().contains(m_widget->mapFromParent(event->pos()))) {
        this->tryHideMainwindow();
    }
}

void MainWindow::paintEvent(QPaintEvent *event) {

//    QPainterPath path;
//    path.addRect(this->rect());
//    KWindowEffects::enableBlurBehind(this->winId(), true, QRegion(path.toFillPolygon().toPolygon()));
    rebuildBackground();
    QPainter p(this);
    p.drawImage(0, 0, m_backgroundImage);
}

void MainWindow::rebuildBackground()
{
    if (!m_backgroundGSetting) {
        return;
    }

    QPoint centerPoint = this->mapToGlobal(this->rect().center());
    QScreen *focusScreen = QGuiApplication::screenAt(centerPoint);
    if ((m_focusScreen == focusScreen) && (focusScreen->size() == m_backgroundImage.size())) {
        return;
    }

    m_focusScreen = focusScreen;

    QImage backgroundImage(m_focusScreen->size(), QImage::Format_ARGB32_Premultiplied);
    backgroundImage.fill(Qt::black);
    QString imagePath = m_backgroundGSetting->get(PICTURE_FILE_NAME).toString();

    if (imagePath.isEmpty()) {
        QColor color(Qt::black);
        QString primaryColor = m_backgroundGSetting->get(PRIMARY_COLOR).toString();
        if (!primaryColor.isEmpty()) {
            color = QColor(primaryColor);
        }
        backgroundImage.fill(color);

    } else {
        QImage loadedImage(imagePath);
        QString pictureOptions = m_backgroundGSetting->get(PICTURE_OPTIONS).toString();
        if (pictureOptions.isEmpty()) {
            pictureOptions = "scaled";
        }
        QPainter painter(&backgroundImage);

        if (pictureOptions == "centered") {
            painter.drawImage((backgroundImage.width() - loadedImage.width()) / 2,
                              (backgroundImage.height() - loadedImage.height()) / 2,
                              loadedImage);

        } else if (pictureOptions == "stretched") {
            painter.drawImage(backgroundImage.rect(), loadedImage, loadedImage.rect());

        } else if (pictureOptions == "scaled") {
            painter.drawImage(backgroundImage.rect(), loadedImage,
                              getSourceRect(backgroundImage.rect(), loadedImage));

        } else if (pictureOptions == "wallpaper") {
            int drewWidth = 0;
            int drewHeight = 0;
            while (true) {
                drewWidth = 0;
                while (true) {
                    painter.drawImage(drewWidth, drewHeight, loadedImage);
                    drewWidth += loadedImage.width();
                    if (drewWidth >= backgroundImage.width()) {
                        break;
                    }
                }
                drewHeight += loadedImage.height();
                if (drewHeight >= backgroundImage.height()) {
                    break;
                }
            }
        } else {
            painter.drawImage(backgroundImage.rect(), loadedImage, loadedImage.rect());
        }
    }

    qt_blurImage(backgroundImage, PICTURE_BLUR_RADIUS, false, 0);

    //以下为修补模糊算法造成的边缘像素透明问题的代码，暂时不用
//    QRect   imgRect = backgroundImage.rect();
//    QRegion resetReg(imgRect);
//    QRegion excReg(imgRect.adjusted(PICTURE_BLUR_RADIUS, PICTURE_BLUR_RADIUS, -PICTURE_BLUR_RADIUS, -PICTURE_BLUR_RADIUS));
//    resetReg = resetReg.subtracted(excReg);
//
//    for (int x = 0; x < imgRect.width(); ++x) {
//        for (int y = 0; y < imgRect.height(); ++y) {
//            if (resetReg.contains({x, y})) {
//                QColor color = backgroundImage.pixelColor(x, y);
//                qDebug() << "==" << x << y << color.alpha();
//                if (color.alpha() <= 128) {
//                    backgroundImage.setPixelColor(x, y, Qt::white);
//                    qDebug() << "==" << x << y << Qt::white;
//                }
//            }
//        }
//    }

    m_backgroundImage.swap(backgroundImage);
}

QRect MainWindow::getSourceRect(const QRect &targetRect, const QImage &image)
{
    qreal screenScale = qreal(targetRect.width()) / qreal(targetRect.height());
    qreal width = image.width();
    qreal height = image.height();

    if ((width / height) == screenScale) {
        return image.rect();
    }

    bool isShortX = (width <= height);
    if (isShortX) {
        screenScale = qreal(targetRect.height()) / qreal(targetRect.width());
    }

    //使用长边与短边目的是屏蔽单独的宽与高概念，减少一部分判断逻辑
    qreal shortEdge = isShortX ? width : height;
    qreal longEdge = isShortX ? height : width;

    while (shortEdge > 1) {
        qint32 temp = qFloor(shortEdge * screenScale);
        if (temp <= longEdge) {
            longEdge = temp;
            break;
        }

        //每次递减5%进行逼近
        qint32 spacing = qRound(shortEdge / 20);
        if (spacing <= 0) {
            spacing = 1;
        }
        shortEdge -= spacing;
    }

    QSize sourceSize = image.size();
    if (shortEdge > 1 && longEdge > 1) {
        sourceSize.setWidth(isShortX ? shortEdge : longEdge);
        sourceSize.setHeight(isShortX ? longEdge : shortEdge);
    }

    qint32 offsetX = 0;
    qint32 offsetY = 0;
    if (image.width() > sourceSize.width()) {
        offsetX = (image.width() - sourceSize.width()) / 2;
    }

    if (image.height() > sourceSize.height()) {
        offsetY = (image.height() - sourceSize.height()) / 2;
    }

    QPoint offsetPoint = image.rect().topLeft();
    offsetPoint += QPoint(offsetX, offsetY);

    return {offsetPoint, sourceSize};
}
