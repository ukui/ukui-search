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
 *
 */
#include "search-detail-view.h"
#include <QPainter>
#include <QStyleOption>
#include <QDebug>
#include <gio/gdesktopappinfo.h>
#include <QDBusInterface>
#include <QDBusReply>
#include <QStandardPaths>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QClipboard>
#include <QApplication>
#include <QFileInfo>
#include <QDateTime>
#include "config-file.h"

SearchDetailView::SearchDetailView(QWidget *parent) : QWidget(parent)
{
    initUI();
}

SearchDetailView::~SearchDetailView()
{
    if (m_layout) {
        delete m_layout;
        m_layout = NULL;
    }
}

/**
 * @brief SearchDetailView::clearLayout 清空布局
 */
void SearchDetailView::clearLayout() {
//    QLayoutItem * child;
//    while ((child = m_layout->takeAt(0)) != 0) {
//        if(child->widget())
//        {
//            child->widget()->setParent(NULL); //防止删除后窗口看上去没有消失
//        }
//        delete child;
//    }
//    child = NULL;
    m_iconLabel->hide();
    m_nameFrame->hide();
    m_nameLabel->hide();
    m_typeLabel->hide();
    m_hLine->hide();
    m_detailFrame->hide();
    m_contentLabel->hide();
    m_pathFrame->hide();
    m_timeFrame->hide();
    m_pathLabel_1->hide();
    m_pathLabel_2->hide();
    m_timeLabel_1->hide();
    m_timeLabel_2->hide();
    m_hLine_2->hide();
    m_optionView->hide();
    m_isEmpty = true;
//    closeWebWidget();
    if (m_webView) {
        m_webView->hide();
    }
//    m_reload = false;
}

/**
 * @brief SearchDetailView::setContent 设置文本区域内容（仅内容搜索）
 * @param text 内容搜索结果组成的文段
 * @param keyword 搜索关键词
 */
void SearchDetailView::setContent(const QString& text, const QString& keyword) {
    m_contentText = text;
    m_keyword = keyword;
}

/**
 * @brief SearchDetailView::isEmpty 返回当前详情页是否为空
 * @return
 */
bool SearchDetailView::isEmpty()
{
    return m_isEmpty;
}

/**
 * @brief SearchDetailView::getType 返回当前详情页显示的类型
 * @return
 */
int SearchDetailView::getType()
{
    return m_type;
}

/**
 * @brief SearchDetailView::setWebWidget 显示为网页
 * @param keyword 关键词
 */
void SearchDetailView::setWebWidget(const QString& keyword)
{
    clearLayout();
    m_isEmpty = false;
    m_reload = false;
    if (m_webView) {
        if (QString::compare(keyword, m_currentKeyword) == 0) { //关键词没有发生变化，只把原来的网页show出来
            m_webView->show();
            return;
        }
    } else {
        m_webView = new QWebEngineView(this);
        m_webView->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
        m_webView->setAttribute(Qt::WA_DeleteOnClose);
        m_webView->move(0, 0);
        m_webView->setFixedSize(378, 522);

        connect(m_webView,&QWebEngineView::loadFinished, this, [ = ](){
            m_reload = true;
        });
        connect(m_webView, &QWebEngineView::urlChanged, this, [ = ](const QUrl& url) {
            if (m_reload) {
                closeWebWidget();
                QDesktopServices::openUrl(url);
            }
        });
    }
    //如果使用非手机版百度跳转，请使用RequestInterceptor类
//    RequestInterceptor * interceptor = new RequestInterceptor(m_webView);
//    QWebEngineProfile * profile = new QWebEngineProfile(m_webView);
//    profile->setRequestInterceptor(interceptor);
//    QWebEnginePage * page = new QWebEnginePage(profile, m_webView);
//    m_webView->setPage(page);

    //新打开网页搜索或关键词发生变化，重新load
    m_currentKeyword = keyword;//目前网页搜索的关键词，记录此词来判断网页是否需要刷新
    QString address;
    QString engine = GlobalSettings::getInstance()->getValue(WEB_ENGINE).toString();
    if (!engine.isEmpty()) {
        if (engine == "360") {
            address = "https://m.so.com/s?q=" + keyword; //360
        } else if (engine == "sougou") {
            address = "https://wap.sogou.com/web/searchList.jsp?&keyword=" + keyword; //搜狗
        } else {
            address = "http://m.baidu.com/s?word=" + keyword; //百度
        }
    } else { //默认值
        address = "http://m.baidu.com/s?word=" + keyword; //百度
    }
//    QString str = "http://m.baidu.com/s?word=" + keyword; //百度
//    QString str = "https://m.so.com/s?q=" + keyword; //360
//    QString str = "https://wap.sogou.com/web/searchList.jsp?&keyword=" + keyword; //搜狗

    m_webView->load(address);
    m_webView->show();
}

void SearchDetailView::setAppWidget(const QString &appname, const QString &path, const QString &iconpath, const QString &description)
{
    m_type = SearchListView::ResType::App;
    m_path = path;
    m_name = appname.contains("/") ? appname.left(appname.indexOf("/")) : appname;
    m_isEmpty = false;
    clearLayout();
    m_iconLabel->show();
    m_nameFrame->show();
    m_nameLabel->show();
    m_typeLabel->show();
    m_hLine->show();

    QIcon icon;
    if (path.isEmpty() || path == "") {
        icon = QIcon(iconpath);
        m_optionView->setupOptions(m_type, false);
        //未安装应用有一个label显示软件描述
        if (description != "" && !description.isEmpty()) {
            m_detailFrame->show();
            m_contentLabel->show();
            m_contentLabel->setText(QString(tr("Introduction: %1")).arg(description));
        }
    } else {
        m_optionView->setupOptions(m_type, true);
        if (QIcon::fromTheme(iconpath).isNull()) {
            icon = QIcon(":/res/icons/desktop.png");
        } else {
            icon = QIcon::fromTheme(iconpath);
        }
    }
    m_optionView->show();

    m_iconLabel->setPixmap(icon.pixmap(icon.actualSize(QSize(96, 96))));
    QFontMetrics fontMetrics = m_nameLabel->fontMetrics();
    QString showname = fontMetrics.elidedText(m_name, Qt::ElideRight, 274); //当字体长度超过215时显示为省略号
    m_nameLabel->setText(showname);
    if (QString::compare(showname, m_name)) {
        m_nameLabel->setToolTip(m_name);
    }
    m_typeLabel->setText(tr("Application"));
}

void SearchDetailView::closeWebWidget()
{
    if (m_webView) {
        m_webView->close();
        m_webView = NULL;
    }
}

QString SearchDetailView::getHtmlText(const QString & text, const QString & keyword) {
    QString htmlString;
    bool boldOpenned = false;
    for (int i = 0; i < text.length(); i++) {
        if ((keyword.toUpper()).contains(QString(text.at(i)).toUpper())) {
            if (! boldOpenned) {
                boldOpenned = true;
                htmlString.append(QString("<b><font size=\"4\">"));
            }
            htmlString.append(escapeHtml(QString(text.at(i))));
        } else {
            if (boldOpenned) {
                boldOpenned = false;
                htmlString.append(QString("</font></b>"));
            }
            htmlString.append(escapeHtml(QString(text.at(i))));
        }
    }
    htmlString.replace("\n", "<br />");//替换换行符
    return htmlString;
}

/**
 * @brief SearchDetailView::escapeHtml 将搜索结果内容中的标签括号转义，来防止文件内容中的标记语言被识别为富文本
 * @param str 需要转义的字段
 * @return
 */
QString SearchDetailView::escapeHtml(const QString & str)
{
    QString temp = str;
    temp.replace("<", "&lt;");
    temp.replace(">", "&gt;");
    return temp;
}

/**
 * @brief SearchDetailView::setupWidget 构建右侧搜索结果详情区域
 * @param type 搜索类型
 * @param path 结果路径
 */
void SearchDetailView::setupWidget(const int& type, const QString& path) {
    m_type = type;
    m_path = path;
    m_isEmpty = false;
    clearLayout();

    m_iconLabel->show();
    m_nameFrame->show();
    m_nameLabel->show();
    m_typeLabel->show();
    m_hLine->show();

    //文件和文件夹有一个额外的详情区域
    if (type == SearchListView::ResType::Dir || type == SearchListView::ResType::File || type == SearchListView::ResType::Content) {
        m_detailFrame->show();
        if (isContent) { //文件内容区域
            m_contentLabel->show();
            m_contentLabel->setText(QApplication::translate("", getHtmlText(m_contentText, m_keyword).toLocal8Bit(), nullptr));
        }
        m_pathFrame->show();
        m_timeFrame->show();
        m_pathLabel_1->show();
        m_pathLabel_2->show();
//        m_pathLabel_2->setText(path);
        QString showPath = path;
        QFontMetrics fontMetrics = m_pathLabel_2->fontMetrics();
        if (fontMetrics.width(path) > m_pathLabel_2->width() - 10) {
            //路径长度超过230,手动添加换行符以实现折叠
            int lastIndex = 0;
            for (int i = lastIndex; i < path.length(); i++) {
                if (fontMetrics.width(path.mid(lastIndex, i - lastIndex)) == m_pathLabel_2->width() - 10) {
                    lastIndex = i;
                    showPath.insert(i, '\n');
                } else if (fontMetrics.width(path.mid(lastIndex, i - lastIndex)) > m_pathLabel_2->width() - 10) {
                    lastIndex = i;
                    showPath.insert(i - 1, '\n');
                } else {
                    continue;
                }
            }
        }
        m_pathLabel_2->setText(showPath);

        m_timeLabel_1->show();
        m_timeLabel_2->show();
        QFileInfo fileInfo(path);
        m_timeLabel_2->setText(fileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss"));
        m_hLine_2->show();
    }

    m_optionView->setupOptions(m_type);
    m_optionView->show();

    //根据不同类型的搜索结果切换加载图片和名称的方式
    switch (type) {
        case SearchListView::ResType::Content:
        case SearchListView::ResType::Dir :
        case SearchListView::ResType::File : {
            QIcon icon = FileUtils::getFileIcon(QString("file://%1").arg(path));
            m_iconLabel->setPixmap(icon.pixmap(icon.actualSize(QSize(96, 96))));
            QFontMetrics fontMetrics = m_nameLabel->fontMetrics();
            QString wholeName = FileUtils::getFileName(path);
            QString name = fontMetrics.elidedText(wholeName, Qt::ElideRight, 274);
            m_nameLabel->setText(name);
            if (QString::compare(name, wholeName)) {
                m_nameLabel->setToolTip(wholeName);
            }
            m_nameLabel->setTextFormat(Qt::PlainText); //显示纯文本
            m_typeLabel->setText(tr("Document"));
            break;
        }
        case SearchListView::ResType::Setting : {
            QIcon icon = FileUtils::getSettingIcon(path, true);
            m_iconLabel->setPixmap(icon.pixmap(icon.actualSize(QSize(96, 96))));
            QString settingType = path.mid(path.indexOf("/") + 1, path.lastIndexOf("/") - path.indexOf("/") - 1); //配置项所属控制面板插件名
            m_nameLabel->setText(settingType);
            m_typeLabel->setText(FileUtils::getSettingName(path));
            break;
        }
        default:
            break;
    }
}

/**
 * @brief SearchDetailView::execActions 根据点击的选项执行指定动作
 * @param type 选中的类型
 */
void SearchDetailView::execActions(const int& type, const int& option, const QString& path) {
    switch (option) {
        case OptionView::Options::Open: {
            if (openAction(type, path)) {
                writeConfigFile(path);
            }
            break;
        }
        case OptionView::Options::Shortcut: {
            addDesktopShortcut(path);
            break;
        }
        case OptionView::Options::Panel: {
            addPanelShortcut(path);
            break;
        }
        case OptionView::Options::OpenPath: {
            openPathAction(path);
            break;
        }
        case OptionView::Options::CopyPath: {
            copyPathAction(path);
            break;
        }
        case OptionView::Options::Install: {
            installAppAction(m_name); //未安装应用点击此选项，不使用路径作为参数，而是使用软件名
        }
        default:
            break;
    }
}

/**
 * @brief SearchDetailView::openAction 执行“打开”动作
 * @return
 */
bool SearchDetailView::openAction(const int& type, const QString& path) {
    switch (type) {
        case SearchListView::ResType::App: {
            GDesktopAppInfo * desktopAppInfo = g_desktop_app_info_new_from_filename(path.toLocal8Bit().data());
            bool res = static_cast<bool>(g_app_info_launch(G_APP_INFO(desktopAppInfo),nullptr, nullptr, nullptr));
            g_object_unref(desktopAppInfo);
            return res;
            break;
        }
        case SearchListView::ResType::Best:
        case SearchListView::ResType::Content:
        case SearchListView::ResType::Dir:
        case SearchListView::ResType::File: {
//            QProcess process;
//            process.startDetached(QString("xdg-open %1").arg(path));
            return QDesktopServices::openUrl(QUrl::fromLocalFile(path));
            break;
        }
        case SearchListView::ResType::Setting: {
            //打开控制面板对应页面
            QProcess  process;
            if (path.left(path.indexOf("/")).toLower() == "wallpaper")
                return process.startDetached(QString("ukui-control-center --background"));
            else  return process.startDetached(QString("ukui-control-center --%1").arg(path.left(path.indexOf("/")).toLower()));
            break;
        }
        default:
            return false;
            break;
    }
}

/**
 * @brief SearchDetailView::writeConfigFile 将打开过的文件或应用或配置项记录下来并发出刷新Homepage的请求
 * @param path 待写入的文件路径
 * @return
 */
bool SearchDetailView::writeConfigFile(const QString& path) {
    if (ConfigFile::writeConfig(path)) {
        Q_EMIT this->configFileChanged();
        return true;
    }
    return false;
}

/**
 * @brief SearchDetailView::initUI 初始化ui
 */
void SearchDetailView::initUI()
{
    m_layout = new QVBoxLayout(this);
    this->setLayout(m_layout);
    m_layout->setContentsMargins(16, 60, 16, 24);
    this->setObjectName("detailView");
    this->setStyleSheet("QWidget#detailView{background:transparent;}");
    this->setFixedWidth(378);

    //图标和名称、分割线区域
    m_iconLabel = new QLabel(this);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    m_iconLabel->setFixedHeight(120);
    m_nameFrame = new QFrame(this);
    m_nameLayout = new QHBoxLayout(m_nameFrame);
    m_nameLabel = new QLabel(m_nameFrame);
    m_typeLabel = new QLabel(m_nameFrame);
    m_nameLabel->setStyleSheet("QLabel{font-size: 18px;}");
    m_typeLabel->setStyleSheet("QLabel{font-size: 14px; color: palette(mid);}");
    m_nameFrame->setFixedHeight(48);
    m_nameLabel->setMaximumWidth(280);
    m_nameLayout->addWidget(m_nameLabel);
    m_nameLayout->addStretch();
    m_nameLayout->addWidget(m_typeLabel);
    m_nameFrame->setLayout(m_nameLayout);
    m_hLine = new QFrame(this);
    m_hLine->setLineWidth(0);
    m_hLine->setFixedHeight(1);
    m_hLine->setStyleSheet("QFrame{background: rgba(0,0,0,0.2);}");
    m_layout->addWidget(m_iconLabel);
    m_layout->addWidget(m_nameFrame);
    m_layout->addWidget(m_hLine);

    //文件和文件夹有一个额外的详情区域
    m_detailFrame = new QFrame(this);
    m_detailLyt = new QVBoxLayout(m_detailFrame);
    m_detailLyt->setContentsMargins(0,0,0,0);

    //文件内容区域
    m_contentLabel = new QLabel(m_detailFrame);
    m_contentLabel->setWordWrap(true);
    m_contentLabel->setContentsMargins(9, 0, 9, 0);
    m_detailLyt->addWidget(m_contentLabel);

    //路径与修改时间区域
    m_pathFrame = new QFrame(m_detailFrame);
    m_timeFrame = new QFrame(m_detailFrame);
    m_pathLyt = new QHBoxLayout(m_pathFrame);
    m_timeLyt = new QHBoxLayout(m_timeFrame);
    m_pathLabel_1 = new QLabel(m_pathFrame);
    m_pathLabel_2 = new QLabel(m_pathFrame);
    m_pathLabel_1->setText(tr("Path"));
    m_pathLabel_2->setFixedWidth(240);
//    m_pathLabel_2->setWordWrap(true);
    m_pathLyt->addWidget(m_pathLabel_1);
    m_pathLyt->addStretch();
    m_pathLyt->addWidget(m_pathLabel_2);
    m_timeLabel_1 = new QLabel(m_timeFrame);
    m_timeLabel_2 = new QLabel(m_timeFrame);
    m_timeLabel_1->setText(tr("Last time modified"));
    m_timeLyt->addWidget(m_timeLabel_1);
    m_timeLyt->addStretch();
    m_timeLyt->addWidget(m_timeLabel_2);
    m_detailLyt->addWidget(m_pathFrame);
    m_detailLyt->addWidget(m_timeFrame);

    m_hLine_2 = new QFrame(this);
    m_hLine_2->setLineWidth(0);
    m_hLine_2->setFixedHeight(1);
    m_hLine_2->setStyleSheet("QFrame{background: rgba(0,0,0,0.2);}");
    m_layout->addWidget(m_detailFrame);
    m_layout->addWidget(m_hLine_2);

    //可执行操作区域
    m_optionView = new OptionView(this);
    connect(m_optionView, &OptionView::onOptionClicked, this, [ = ](const int& option) {
        execActions(m_type, option, m_path);
    });
    m_layout->addWidget(m_optionView);

    m_layout->addStretch();

    this->clearLayout(); //初始化时隐藏所有控件
}

/**
 * @brief SearchDetailView::addDesktopShortcut 添加到桌面快捷方式
 * @return
 */
bool SearchDetailView::addDesktopShortcut(const QString& path) {
    QString dirpath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QFileInfo fileInfo(path);
    QString desktopfn = fileInfo.fileName();
    QFile file(path);
    QString newName = QString(dirpath+"/"+desktopfn);
    bool ret = file.copy(QString(dirpath+"/"+desktopfn));
    if(ret)
    {
        QProcess process;
        process.startDetached(QString("chmod a+x %1").arg(newName));
        return true;
    }
    return false;
}

/**
 * @brief SearchDetailView::addPanelShortcut 添加到任务栏
 * @return
 */
bool SearchDetailView::addPanelShortcut(const QString& path) {
    QDBusInterface iface("com.ukui.panel.desktop",
                         "/",
                         "com.ukui.panel.desktop",
                         QDBusConnection::sessionBus());
    if (iface.isValid()) {
        QDBusReply<bool> isExist = iface.call("CheckIfExist",path);
        if (isExist) {
            qDebug()<<"qDebug: Add shortcut to panel failed, because it is already existed!";
            return false;
        }
        QDBusReply<QVariant> ret = iface.call("AddToTaskbar",path);
        qDebug()<<"qDebug: Add shortcut to panel successed!";
        return true;
    }
    return false;
}

/**
 * @brief SearchDetailView::openPathAction 打开文件所在路径
 * @return
 */
bool SearchDetailView::openPathAction(const QString& path) {
    return QDesktopServices::openUrl(QUrl::fromLocalFile(path.left(path.lastIndexOf("/"))));
}

/**
 * @brief SearchDetailView::copyPathAction 复制文件所在路径
 * @return
 */
bool SearchDetailView::copyPathAction(const QString& path) {
    QClipboard * clipboard = QApplication::clipboard();   //获取系统剪贴板指针
    clipboard->setText(path);
    return true;
}

/**
 * @brief SearchDetailView::installAppAction 执行打开软件商店操作
 * @param name
 * @return
 */
bool SearchDetailView::installAppAction(const QString & name)
{
    //打开软件商店下载此软件
    QProcess process;
    QString app_name = name.contains("/") ? name.mid(name.indexOf("/") + 1) : name;
    bool res = process.startDetached(QString("kylin-software-center -find %1").arg(app_name));
    return res;
}

void SearchDetailView::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QRect rect = this->rect();
    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    p.setBrush(opt.palette.color(QPalette::Text));
    p.setOpacity(0.06);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rect, 4, 4);
    return QWidget::paintEvent(event);
}

/**
 * @brief RequestInterceptor::interceptRequest 拦截qwebengineview的Url请求
 * @param info
 */
//void RequestInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
//{
//当使用PC版搜索引擎时，可以使用此方法获取用户点击的链接的url
//     QUrl url = info.requestUrl();
//     qDebug() <<"Request URL:" <<url;
//}
