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
#include "libsearch.h"
#include "kwindowsystem.h"

//#include "inotify-manager.h"
#include "settings-widget.h"

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
//    FileUtils::findMultiToneWords("仇仇仇仇仇仇仇仇仇仇仇翟康宁test");
    /*-------------Inotify Test Start---------------*/
    QTime t1 = QTime::currentTime();
    InotifyManagerRefact* im = new InotifyManagerRefact("/home");
    im->Traverse();
    QTime t2 = QTime::currentTime();
    qDebug() << t1;
    qDebug() << t2;
    im->start();
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
            m_contentFrame->setCurrentIndex(0);
        } else {
            m_contentFrame->setCurrentIndex(1);
            QTimer::singleShot(50,this,[=](){
               searchContent(text);
            });
        }
    });

    //初始化homepage
    QVector<QStringList> lists;

    //测试用数据
    QStringList list;
    list<<"/usr/share/applications/peony.desktop"<<"/usr/share/applications/ukui-control-center.desktop"<<"/usr/share/applications/ukui-clock.desktop"<<"/usr/share/applications/wps-office-pdf.desktop";
    QStringList list2;
    list2<<QString("%1/下载").arg(QDir::homePath())<<"/home/zjp/ukui/NM/v10/kylin-nm/README.md"<<"Theme/主题/更改壁纸"<<"/home/zjp/下载/WiFi_AP选择.docx";
    QStringList list3;
    list3<<"/usr/share/applications/peony.desktop"<<"/usr/share/applications/ukui-control-center.desktop"<<"Theme/主题/更改壁纸";

    lists.append(list);
    lists.append(list2);
    lists.append(list3);

    //将搜索结果加入列表
    m_contentFrame->initHomePage(lists);
}

/**
 * @brief bootOptionsFilter 过滤终端命令
 * @param opt
 */
void MainWindow::bootOptionsFilter(QString opt)
{

    if (opt == "-s" || opt == "-show") {
        clearSearchResult();
        this->show();
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
//    QVector<int> types;
//    QVector<QStringList> lists;
    m_lists.clear();
    m_types.clear();

    AppMatch * appMatchor = new AppMatch(this);
    SettingsMatch * settingMatchor = new SettingsMatch(this);

    //测试用数据
    QStringList list;
    list = appMatchor->startMatchApp(searchcontent);
//    list<<"/usr/share/applications/peony.desktop"<<"/usr/share/applications/ukui-control-center.desktop"<<"/usr/share/applications/wps-office-pdf.desktop";
//    QStringList list2;
//    list2<<"/home/zjp/下载/搜索结果.png"<<"/home/zjp/下载/显示不全.mp4"<<"/home/zjp/下载/dmesg.log"<<"/home/zjp/下载/WiFi_AP选择.docx";
    QStringList list3;
    list3 = settingMatchor->startMatchApp(searchcontent);
//    list3<<"About/关于/计算机属性"<<"Area/语言和地区/货币单位"<<"Datetime/时间和日期/手动更改时间"<<"Theme/主题/图标主题";
    m_types.append(SearchItem::SearchType::Apps);
    m_types.append(SearchItem::SearchType::Settings);
//    types.append(SearchItem::SearchType::Files);

    m_lists.append(list);
    m_lists.append(list3);
//    lists.append(list2);
//    m_contentFrame->refreshSearchList(m_types, m_lists);

    //文件搜索

    FileSearcher *searcher = new FileSearcher();

    connect(searcher,&FileSearcher::result,[=](QVector<QStringList> resultV){

        QStringList list1 = resultV.at(0);
        QStringList list2 = resultV.at(1);

//        QVector<QStringList> lists;
        m_lists.append(list1);
        m_lists.append(list2);
//        QVector<int> types;
        m_types.append(SearchItem::SearchType::Dirs);
        m_types.append(SearchItem::SearchType::Files);
        m_contentFrame->refreshSearchList(m_types, m_lists);
    });
    searcher->onKeywordSearch(searchcontent,0,10);
//    QStringList res = IndexGenerator::IndexSearch(searchcontent);
//    types.append(SearchItem::SearchType::Files);
//    lists.append(res);

    //将搜索结果加入列表
//    m_contentFrame->refreshSearchList(types, lists);
}

//使用GSetting获取当前窗口应该使用的透明度
double MainWindow::getTransparentData()
{
    if (!m_transparency_gsettings) {
        return 0.7;
    }

    QStringList keys = m_transparency_gsettings->keys();
    if (keys.contains("transparency")) {
        double tp = m_transparency_gsettings->get("transparency").toDouble();
        return tp;
    } else {
        return 0.7;
    }
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
    qDebug()<<"YYF - event->response_type : "<<event->response_type;//YYF 20200922
    case XCB_FOCUS_OUT:
        this->close();
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
