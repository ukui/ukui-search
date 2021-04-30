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
#include <QtX11Extras/QX11Info>
#include <syslog.h>
#include <QPalette>
#include <QScreen>
#include <QStyleOption>
#include <KWindowEffects>
#include <QPixmap>
#include "kwindowsystem.h"
#include "qt-single-application.h"

//#include "inotify-manager.h"
#include "settings-widget.h"


#include "global-settings.h"
#include "search-result.h"

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

    m_searcher = new SearchManager(this);
    m_settingsMatch = new SettingsMatch(this);

//    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
//    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    this->setWindowIcon(QIcon::fromTheme("kylin-search"));
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setAutoFillBackground(false);
    this->setFocusPolicy(Qt::StrongFocus);
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    this->setWindowTitle(tr("ukui-search"));
    initUi();
    initTimer();

//    setProperty("useStyleWindowManager", false); //禁止拖动
    m_hints.flags = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
    m_hints.functions = MWM_FUNC_ALL;
    m_hints.decorations = MWM_DECOR_BORDER;
    XAtomHelper::getInstance()->setWindowMotifHint(winId(), m_hints);

    QPainterPath path;
    auto rect = this->rect();
    rect.adjust(1, 1, -1, -1);
//    path.addRoundedRect(rect, 0, 0);
    path.addRect(rect);
    setProperty("blurRegion", QRegion(path.toFillPolygon().toPolygon()));
    KWindowEffects::enableBlurBehind(this->winId(), true, QRegion(path.toFillPolygon().toPolygon()));

    connect(qApp, &QApplication::paletteChanged, this, [ = ](const QPalette & pal) {
        this->setPalette(pal);
        this->update();
        Q_FOREACH(QWidget *widget, this->findChildren<QWidget *>()) {
            if(widget) {
                widget->update();
            }
        }
    });

    m_search_result_file = new QQueue<QString>;
    m_search_result_dir = new QQueue<QString>;
    m_search_result_content = new QQueue<QPair<QString, QStringList>>;
    m_search_result_thread = new SearchResult(this);
    m_seach_app_thread = new SearchAppThread(this);
//    m_search_result_thread->start();
    connect(m_search_result_thread, &SearchResult::searchResultFile, this, [ = ](QString path) {
//        qDebug()<<"Append a file into list: "<<path;
        m_contentFrame->appendSearchItem(SearchItem::SearchType::Files, path);
    });
    connect(m_search_result_thread, &SearchResult::searchResultDir, this, [ = ](QString path) {
//        qDebug()<<"Append a dir into list: "<<path;
        m_contentFrame->appendSearchItem(SearchItem::SearchType::Dirs, path);
    });
    connect(m_search_result_thread, &SearchResult::searchResultContent, this, [ = ](QPair<QString, QStringList> pair) {
//        qDebug()<<"Append a file content into list: "<<pair.first;
        m_contentFrame->appendSearchItem(SearchItem::SearchType::Contents, pair.first, pair.second);
    });
    qRegisterMetaType<QVector<QStringList>>("QVector<QStringList>");
    connect(m_seach_app_thread, &SearchAppThread::searchResultApp, this, [ = ](const QVector<QStringList>& applist) {
        qDebug() << "Append applist: " << applist;
        m_contentFrame->setAppList(applist);
    });

    m_sys_tray_icon = new QSystemTrayIcon(this);
    m_sys_tray_icon->setIcon(QIcon::fromTheme("system-search-symbolic"));
    m_sys_tray_icon->setToolTip(tr("Global Search"));
    m_sys_tray_icon->show();
    connect(m_sys_tray_icon, &QSystemTrayIcon::activated, this, [ = ](QSystemTrayIcon::ActivationReason reason) {
        if(reason == QSystemTrayIcon::Trigger) {
            if(!this->isVisible()) {
                clearSearchResult();
//                this->moveToPanel();
                centerToScreen(this);
                XAtomHelper::getInstance()->setWindowMotifHint(winId(), m_hints);
                this->show();
                this->m_searchLayout->focusIn(); //打开主界面时输入框夺焦，可直接输入
                this->raise();
                this->activateWindow();
            } else {
                this->hide();
                m_contentFrame->closeWebView();
                m_search_result_thread->requestInterruption();
                m_search_result_thread->quit();
//                m_seach_app_thread->stop();
            }
        }
    });
    QObject::connect(this, &MainWindow::searchMethodChanged, this, [ = ](FileUtils::SearchMethod sm) {
        this->m_searchMethodManager.searchMethod(sm);
    });

}

MainWindow::~MainWindow() {
    if(m_searchWidget) {
        delete m_searchWidget;
        m_searchWidget = NULL;
    }
    if(m_searchLayout) {
        delete m_searchLayout;
        m_searchLayout = NULL;
    }
    if(m_settingsWidget) {
        delete m_settingsWidget;
        m_settingsWidget = NULL;
    }
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
 * @brief initUi
 * 设置本窗口的大小 this->setFixedSize(640, 640);
 */
void MainWindow::initUi() {
    this->setFixedSize(640, 590);

    m_frame = new QFrame(this);

    this->setCentralWidget(m_frame);
    QVBoxLayout * mainlayout = new QVBoxLayout(m_frame);
    mainlayout->setContentsMargins(8, 0, 8, 6);
    m_frame->setLayout(mainlayout);

    m_titleFrame = new QFrame(m_frame);//标题栏
    m_titleFrame->setFixedHeight(40);
    m_titleLyt = new QHBoxLayout(m_titleFrame);
    m_titleLyt->setContentsMargins(0, 0, 0, 0);
    m_iconLabel = new QLabel(m_titleFrame);
    m_iconLabel->setFixedSize(24, 24);
    m_iconLabel->setPixmap(QIcon::fromTheme("kylin-search").pixmap(QSize(24, 24)));
    m_titleLabel = new QLabel(m_titleFrame);
    m_titleLabel->setText(tr("Search"));
    m_menuBtn = new QPushButton(m_titleFrame);
    m_menuBtn->setFixedSize(30, 30);
//    m_menuBtn->setIcon(QIcon(":/res/icons/commonuse.svg"));
    m_menuBtn->setIcon(QIcon::fromTheme("document-properties-symbolic"));
    m_menuBtn->setProperty("useIconHighlightEffect", 0x2);
    m_menuBtn->setProperty("isWindowButton", 0x01);
    m_menuBtn->setFlat(true);
    connect(m_menuBtn, &QPushButton::clicked, this, [ = ]() {
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
    });
    m_titleLyt->addWidget(m_iconLabel);
    m_titleLyt->addWidget(m_titleLabel);
    m_titleLyt->addStretch();
    m_titleLyt->addWidget(m_menuBtn);

    m_contentFrame = new ContentWidget(m_frame);//内容栏

    m_searchWidget = new SeachBarWidget(this);
    m_searchLayout = new SearchBarHLayout(this);
    m_searchWidget->setLayout(m_searchLayout);
    m_searchWidget->setFixedHeight(44);

    mainlayout->addWidget(m_titleFrame);
    mainlayout->addWidget(m_contentFrame);
    mainlayout->addWidget(m_searchWidget);
    connect(m_contentFrame, &ContentWidget::mousePressed, m_searchLayout, &SearchBarHLayout::effectiveSearchRecord);

    connect(QApplication::primaryScreen(), &QScreen::geometryChanged,
            this, &MainWindow::monitorResolutionChange);
    connect(qApp, &QApplication::primaryScreenChanged, this,
            &MainWindow::primaryScreenChangedSlot);
    connect(m_searchLayout, &SearchBarHLayout::textChanged, this, [ = ](QString text) {
        if(text == "") {
            if(m_search_result_thread->isRunning()) {
                m_search_result_thread->requestInterruption();
                m_search_result_thread->quit();
            }
//            m_seach_app_thread->stop();
            m_contentFrame->setCurrentIndex(0);
            m_askTimer->stop();
        } else {
            m_contentFrame->setCurrentIndex(1);
            QTimer::singleShot(10, this, [ = ]() {
                m_search_result_file->clear();
                m_search_result_dir->clear();
                m_search_result_content->clear();
                if(! m_search_result_thread->isRunning()) {
                    m_search_result_thread->start();
                }
                searchContent(text);
                //允许弹窗且当前次搜索（为关闭主界面，算一次搜索过程）未询问且当前为暴力搜索
                if(GlobalSettings::getInstance()->getValue(ENABLE_CREATE_INDEX_ASK_DIALOG).toString() != "false" && !m_currentSearchAsked && FileUtils::searchMethod == FileUtils::SearchMethod::DIRECTSEARCH)
                    m_askTimer->start();
            });
        }
        m_researchTimer->stop(); //如果搜索内容发生改变，则停止建索引后重新搜索的倒计时
    });

    //初始化homepage
//    m_contentFrame->setQuicklyOpenList(list); //如需自定义快捷打开使用本函数
    m_contentFrame->initHomePage();

    //创建索引询问弹窗
    m_askDialog = new CreateIndexAskDialog(this);
    MotifWmHints ask_dialog_hints;
    ask_dialog_hints.flags = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
    ask_dialog_hints.functions = MWM_FUNC_ALL;
    ask_dialog_hints.decorations = MWM_DECOR_BORDER;
    XAtomHelper::getInstance()->setWindowMotifHint(m_askDialog->winId(), ask_dialog_hints);
    connect(m_askDialog, &CreateIndexAskDialog::closed, this, [ = ]() {
        m_isAskDialogVisible = false;
    });
    connect(m_askDialog, &CreateIndexAskDialog::btnClicked, this, [ = ](const bool & create_index, const bool & no_longer_ask) {
        if(no_longer_ask) {
            GlobalSettings::getInstance()->setValue(ENABLE_CREATE_INDEX_ASK_DIALOG, "false");
        } else {
            GlobalSettings::getInstance()->setValue(ENABLE_CREATE_INDEX_ASK_DIALOG, "true");
        }
        if(create_index) {
            if(m_search_gsettings && m_search_gsettings->keys().contains(SEARCH_METHOD_KEY)) {
                m_search_gsettings->set(SEARCH_METHOD_KEY, true);
            } else {
                //调用创建索引接口
                Q_EMIT this->searchMethodChanged(FileUtils::SearchMethod::INDEXSEARCH);
                //创建索引十秒后重新搜索一次(如果用户十秒内没有退出搜索界面且没有重新搜索)
                m_researchTimer->start();
            }
        }
    });
    installEventFilter(this);
}

/**
 * @brief bootOptionsFilter 过滤终端命令
 * @param opt
 */
void MainWindow::bootOptionsFilter(QString opt) {
    if(opt == "-s" || opt == "--show") {
        clearSearchResult();
//        this->moveToPanel();
        centerToScreen(this);
        XAtomHelper::getInstance()->setWindowMotifHint(winId(), m_hints);
        this->show();
        this->m_searchLayout->focusIn();
        this->raise();
        this->activateWindow();
//        m_search_result_thread->start();
    }
}

/**
 * @brief clearSearchResult 清空搜索结果
 */
void MainWindow::clearSearchResult() {
    m_searchLayout->clearText();
    m_searchLayout->focusOut();
}

/**
 * @brief MainWindow::createIndexSlot 允许创建索引的槽函数
 */
void MainWindow::createIndexSlot() {
}

/**
 * @brief loadMainWindow 加载主界面的函数
 * 不删除的原因是在单例和main函数里面需要用
 */

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
 * @brief searchContent 搜索关键字
 * @param searchcontent
 */
void MainWindow::searchContent(QString keyword) {
    m_contentFrame->setKeyword(keyword);

    //设置搜索
    QStringList settingList;
    settingList = m_settingsMatch->startMatchApp(keyword);
    m_contentFrame->resetSearchList();
    m_contentFrame->setSettingList(settingList);

    //应用搜索
//    m_seach_app_thread->stop();
    m_seach_app_thread->startSearch(keyword);

    //文件、文件夹、内容搜索
    this->m_searcher->onKeywordSearch(keyword, m_search_result_file, m_search_result_dir, m_search_result_content);
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
    QDBusInterface primaryScreenInterface("org.ukui.SettingsDaemon",
                                          "/org/ukui/SettingsDaemon/wayland",
                                          "org.ukui.SettingsDaemon.wayland",
                                          QDBusConnection::sessionBus());
    if(QDBusReply<int>(primaryScreenInterface.call("x")).isValid()) {
        QDBusReply<int> width = primaryScreenInterface.call("width");
        QDBusReply<int> height = primaryScreenInterface.call("height");
        desk_x = width;
        desk_y = height;
    }
    widget->move(desk_x / 2 - x / 2 + desk_rect.left(), desk_y / 2 - y / 2 + desk_rect.top());
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
    m_askTimer->setInterval(5 * 1000);
    connect(m_askTimer, &QTimer::timeout, this, [ = ]() {
        if(this->isVisible()) {
            m_isAskDialogVisible = true;
            m_askDialog->show();
            m_currentSearchAsked = true;
        }
        m_askTimer->stop();
    });
    m_researchTimer = new QTimer;
    m_researchTimer->setInterval(10 * 1000);
    connect(m_researchTimer, &QTimer::timeout, this, [ = ]() {
        if(this->isVisible()) {
            searchContent(m_searchLayout->text());
        }
        m_researchTimer->stop();
    });
}

/**
 * @brief MainWindow::setSearchMethod 设置搜索模式
 * @param is_index_search true为索引搜索，false为暴力搜索
 */
void MainWindow::setSearchMethod(const bool &is_index_search) {
    if(is_index_search) {
        //调用创建索引接口
        Q_EMIT this->searchMethodChanged(FileUtils::SearchMethod::INDEXSEARCH);
        //创建索引十秒后重新搜索一次(如果用户十秒内没有退出搜索界面且没有重新搜索)
        m_researchTimer->start();
    } else {
        Q_EMIT this->searchMethodChanged(FileUtils::SearchMethod::DIRECTSEARCH);
        m_researchTimer->stop();
    }
}

/**
 * @brief MainWindow::nativeEvent 处理窗口失焦事件
 * @param eventType
 * @param message
 * @param result
 * @return
 */
//bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
//{
//    Q_UNUSED(result);
//    if (eventType != "xcb_generic_event_t") {
//        return false;
//    }

//    xcb_generic_event_t *event = (xcb_generic_event_t*)message;

//    switch (event->response_type & ~0x80) {
//    case XCB_FOCUS_OUT:
//        if (!m_isAskDialogVisible) {
//            m_currentSearchAsked = false;
//            this->hide();
//            m_askTimer->stop();
//            m_researchTimer->stop();
//            m_contentFrame->closeWebView();
//            m_search_result_thread->requestInterruption();
//            m_search_result_thread->quit();
//        }
////        m_seach_app_thread->stop();
//        break;
//    default:
//        break;
//    }

//    return false;
//}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Escape) {
        this->hide();
        m_contentFrame->closeWebView();
        m_search_result_thread->requestInterruption();
        m_search_result_thread->quit();
//        m_seach_app_thread->stop();
    }
    return QWidget::keyPressEvent(event);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::ActivationChange) {
        if(QApplication::activeWindow() != this) {
            if (!m_isAskDialogVisible) {
                m_currentSearchAsked = false;
                this->hide();
                m_askTimer->stop();
                m_researchTimer->stop();
                m_contentFrame->closeWebView();
                m_search_result_thread->requestInterruption();
            }
        }
    }
    return QMainWindow::eventFilter(watched,event);
}

void MainWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)

    double trans = getTransparentData();
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QRect rect = this->rect();
    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    p.setBrush(opt.palette.color(QPalette::Base));
    p.setOpacity(trans);
    p.setPen(Qt::NoPen);
//    p.drawRoundedRect(rect, 6, 6);
    p.drawRect(rect);
    return QWidget::paintEvent(event);

}
