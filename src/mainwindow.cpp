/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */

#include "mainwindow.h"
#include <QHBoxLayout>
#include <QDebug>
#include <QDesktopWidget>
#include <QtSingleApplication>
#include <QtX11Extras/QX11Info>
#include <syslog.h>
#include <QPalette>
#include <QScreen>
#include <QStyleOption>
#include <KWindowEffects>
#include <QPixmap>
#include "kwindowsystem.h"

//#include "inotify-manager.h"
#include "settings-widget.h"


#include "global-settings.h"
#include "search-result.h"

extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);
/**
 * @brief MainWindow 主界面
 * @param parent
 *
 * 慎用KWindowSystem::setShowingDesktop(!KWindowSystem::showingDesktop());
 * 可能造成窗口属性的混乱
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{

    m_searcher = new FileSearcher();
    //    FileUtils::findMultiToneWords("仇仇仇仇仇仇仇仇仇仇仇翟康宁test");
    /*-------------Inotify Test Start---------------*/
    //    QTime t1 = QTime::currentTime();
    //    InotifyManagerRefact* im = new InotifyManagerRefact("/home");
    //    im->Traverse();
    //    QTime t2 = QTime::currentTime();
    //    qDebug() << t1;
    //    qDebug() << t2;
    //    im->start();
    /*-------------Inotify Test End-----------------*/

    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setAutoFillBackground(false);
    this->setFocusPolicy(Qt::StrongFocus);
    this->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    initUi();

    QPainterPath path;
    auto rect = this->rect();
    rect.adjust(1, 1, -1, -1);
    path.addRoundedRect(rect, 6, 6);
    setProperty("blurRegion", QRegion(path.toFillPolygon().toPolygon()));
    KWindowEffects::enableBlurBehind(this->winId(), true, QRegion(path.toFillPolygon().toPolygon()));

    const QByteArray id("org.ukui.control-center.personalise");
    if (QGSettings::isSchemaInstalled(id)) {
        m_transparency_gsettings = new QGSettings(id);
    }

    connect(qApp, &QApplication::paletteChanged, this, [ = ](const QPalette &pal) {
        this->setPalette(pal);
        this->update();
    });

    m_search_result_file = new QQueue<QString>;
    m_search_result_dir = new QQueue<QString>;
    m_search_result_content = new QQueue<QPair<QString,QStringList>>;
    m_search_result_thread = new SearchResult(this);
//    m_search_result_thread->start();
    connect(m_search_result_thread, &SearchResult::searchResultFile, this, [ = ](QString path) {
        m_contentFrame->appendSearchItem(SearchItem::SearchType::Files, path, m_searchLayout->text());
    });
    connect(m_search_result_thread, &SearchResult::searchResultDir, this, [ = ](QString path) {
        m_contentFrame->appendSearchItem(SearchItem::SearchType::Dirs, path, m_searchLayout->text());
    });
    connect(m_search_result_thread, &SearchResult::searchResultContent, this, [ = ](QPair<QString, QStringList> pair) {
        m_contentFrame->appendSearchItem(SearchItem::SearchType::Contents, pair.first, m_searchLayout->text(), pair.second);
    });

    m_sys_tray_icon = new QSystemTrayIcon(this);
    m_sys_tray_icon->setIcon(QIcon::fromTheme("system-search-symbolic"));
    m_sys_tray_icon->setToolTip(tr("Global Search"));
    m_sys_tray_icon->show();
    connect(m_sys_tray_icon,&QSystemTrayIcon::activated,this,[=](QSystemTrayIcon::ActivationReason reason){
        if(reason == QSystemTrayIcon::Trigger)
        {
            clearSearchResult();
            this->show();
            this->raise();
            this->activateWindow();
        }
    });
}

MainWindow::~MainWindow()
{
    if (m_searchWidget) {
        delete m_searchWidget;
        m_searchWidget = NULL;
    }
    if (m_searchLayout) {
        delete m_searchLayout;
        m_searchLayout = NULL;
    }
    if (m_settingsWidget) {
        delete m_settingsWidget;
        m_settingsWidget = NULL;
    }
}

/**
 * @brief initUi
 * 设置本窗口的大小 this->setFixedSize(640, 640);
 */
void MainWindow::initUi()
{
    this->setFixedSize(640, 640);

    m_frame = new QFrame(this);

    this->setCentralWidget(m_frame);
    QVBoxLayout * mainlayout = new QVBoxLayout(m_frame);
    mainlayout->setContentsMargins(16, 0, 16, 16);
    m_frame->setLayout(mainlayout);
    m_frame->setStyleSheet("QLabel{color: palette(text);}");

    m_titleFrame = new QFrame(m_frame);//标题栏
    m_titleFrame->setFixedHeight(48);
    m_titleLyt = new QHBoxLayout(m_titleFrame);
    m_titleLyt->setContentsMargins(0, 0, 0, 0);
    m_iconLabel = new QLabel(m_titleFrame);
    m_iconLabel->setFixedSize(20, 20);
    m_iconLabel->setPixmap(QPixmap(":/res/icons/edit-find-symbolic.svg"));
    m_titleLabel = new QLabel(m_titleFrame);
    m_titleLabel->setText(tr("Search"));
    m_menuBtn = new QPushButton(m_titleFrame);
    m_menuBtn->setFixedSize(24, 24);
    m_menuBtn->setIcon(QIcon(":/res/icons/commonuse.svg"));
    m_menuBtn->setStyleSheet("QPushButton{background: transparent;}"
                             "QPushButton:hover:!pressed{background: transparent;}");
    connect(m_menuBtn, &QPushButton::clicked, this, [ = ]() {
        if (m_settingsWidget) { //当此窗口已存在时，仅需置顶
            Qt::WindowFlags flags = m_settingsWidget->windowFlags();
            flags |= Qt::WindowStaysOnTopHint;
            m_settingsWidget->setWindowFlags(flags);
            flags &= ~Qt::WindowStaysOnTopHint;
            m_settingsWidget->setWindowFlags(flags);
            m_settingsWidget->show();
            return;
        }
        m_settingsWidget = new SettingsWidget();
        m_settingsWidget->show();
        connect(m_settingsWidget, &SettingsWidget::settingWidgetClosed, this, [ = ]() {
            QTimer::singleShot(100, this, [ = ] {
                clearSearchResult();
//                m_search_result_thread->start();
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

    m_searchWidget = new UKuiSeachBarWidget;
    m_searchLayout = new UkuiSearchBarHLayout;
    m_searchWidget->setLayout(m_searchLayout);
    m_searchWidget->setFixedHeight(44);

    mainlayout->addWidget(m_titleFrame);
    mainlayout->addWidget(m_contentFrame);
    mainlayout->addWidget(m_searchWidget);

    connect(QApplication::primaryScreen(), &QScreen::geometryChanged,
            this, &MainWindow::monitorResolutionChange);
    connect(qApp, &QApplication::primaryScreenChanged, this,
            &MainWindow::primaryScreenChangedSlot);
    connect(m_searchLayout, &UkuiSearchBarHLayout::textChanged, this, [ = ](QString text) {
        if (text == "") {
            if (m_search_result_thread->isRunning()) {
                m_search_result_thread->requestInterruption();
                m_search_result_thread->quit();
            }
            m_contentFrame->setCurrentIndex(0);
        } else {
            m_contentFrame->setCurrentIndex(1);
//            QTimer::singleShot(50,this,[=](){
                if (! m_search_result_thread->isRunning()) {
                    m_search_result_thread->start();
                }
                searchContent(text);
//            });
        }
    });

    //初始化homepage
    m_contentFrame->initHomePage();
}

/**
 * @brief bootOptionsFilter 过滤终端命令
 * @param opt
 */
void MainWindow::bootOptionsFilter(QString opt)
{
    if (opt == "-s" || opt == "--show") {
        clearSearchResult();
        this->show();
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
}

/**
 * @brief loadMainWindow 加载主界面的函数
 * 不删除的原因是在单例和main函数里面需要用
 */

/**
 * @brief monitorResolutionChange  监听屏幕改变
 * @param rect
 */
void MainWindow::monitorResolutionChange(QRect rect)
{
    Q_UNUSED(rect);
}

/**
 * @brief primaryScreenChangedSlot 监听分辨率改变
 * @param screen
 */
void MainWindow::primaryScreenChangedSlot(QScreen *screen)
{
    Q_UNUSED(screen);

}

/**
 * @brief searchContent 搜索关键字
 * @param searchcontent
 */
void MainWindow::searchContent(QString searchcontent){
    m_lists.clear();
    m_types.clear();

    AppMatch * appMatchor = new AppMatch(this);
    SettingsMatch * settingMatchor = new SettingsMatch(this);
    //应用与设置搜索
    QStringList appList;
    appList = appMatchor->startMatchApp(searchcontent);
    QStringList settingList;
    settingList = settingMatchor->startMatchApp(searchcontent);
    m_types.append(SearchItem::SearchType::Apps);
    m_types.append(SearchItem::SearchType::Settings);
    m_lists.append(appList);
    m_lists.append(settingList);
    m_contentFrame->refreshSearchList(m_types, m_lists, searchcontent);
  
    //文件、文件夹、内容搜索
    this->m_searcher->onKeywordSearch(searchcontent, m_search_result_file, m_search_result_dir, m_search_result_content);
}

//使用GSetting获取当前窗口应该使用的透明度
double MainWindow::getTransparentData()
{
    return GlobalSettings::getInstance()->getValue(TRANSPARENCY_KEY).toDouble();
}

/**
 * @brief MainWindow::nativeEvent 处理窗口失焦事件
 * @param eventType
 * @param message
 * @param result
 * @return
 */
bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(result);
    if (eventType != "xcb_generic_event_t") {
        return false;
    }

    xcb_generic_event_t *event = (xcb_generic_event_t*)message;

    switch (event->response_type & ~0x80) {
    case XCB_FOCUS_OUT:
        this->hide();
        m_search_result_thread->requestInterruption();
        m_search_result_thread->quit();
        break;
    }

    return false;
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
    p.drawRoundedRect(rect, 6, 6);
    QWidget::paintEvent(event);

}
