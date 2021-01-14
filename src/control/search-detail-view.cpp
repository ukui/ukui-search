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
    m_layout = new QVBoxLayout(this);
    this->setLayout(m_layout);
    m_layout->setContentsMargins(16, 60, 16, 24);
    this->setObjectName("detailView");
    this->setStyleSheet("QWidget#detailView{background:transparent;}");
    this->setFixedWidth(360);
}

SearchDetailView::~SearchDetailView()
{
    if (m_layout) {
        clearLayout();
        delete m_layout;
        m_layout = NULL;
    }
}

/**
 * @brief SearchDetailView::clearLayout 清空布局
 */
void SearchDetailView::clearLayout() {
    QLayoutItem * child;
    while ((child = m_layout->takeAt(0)) != 0) {
        if(child->widget())
        {
            child->widget()->setParent(NULL); //防止删除后窗口看上去没有消失
        }
        delete child;
    }
    child = NULL;
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

QString SearchDetailView::getHtmlText(const QString & text, const QString & keyword) {
    QString htmlString;
    bool boldOpenned = false;
    for (int i = 0; i < text.length(); i++) {
        if ((keyword.toUpper()).contains(QString(text.at(i)).toUpper())) {
            if (! boldOpenned) {
                boldOpenned = true;
                htmlString.append(QString("<b><font size=\"4\">"));
            }
            htmlString.append(QString(text.at(i)));
        } else {
            if (boldOpenned) {
                boldOpenned = false;
                htmlString.append(QString("</font></b>"));
            }
            htmlString.append(QString(text.at(i)));
        }
    }
    htmlString.replace("\n", "<br />");//替换换行符
    return htmlString;
}

/**
 * @brief SearchDetailView::setupWidget 构建右侧搜索结果详情区域
 * @param type 搜索类型
 * @param path 结果路径
 */
void SearchDetailView::setupWidget(const int& type, const QString& path) {
    clearLayout();

    //图标和名称、分割线区域
    QLabel * iconLabel = new QLabel(this);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setFixedHeight(120);

    QFrame * nameFrame = new QFrame(this);
    QHBoxLayout * nameLayout = new QHBoxLayout(nameFrame);
    QLabel * nameLabel = new QLabel(nameFrame);
    QLabel * typeLabel = new QLabel(nameFrame);
    nameLabel->setStyleSheet("QLabel{font-size: 18px;}");
//    typeLabel->setStyleSheet("QLabel{font-size: 14px; color: rgba(0, 0, 0, 0.43);}");
    typeLabel->setStyleSheet("QLabel{font-size: 14px; color: palette(mid);}");
    nameFrame->setFixedHeight(48);
    nameLabel->setMaximumWidth(240);
    nameLayout->addWidget(nameLabel);
    nameLayout->addStretch();
    nameLayout->addWidget(typeLabel);
    nameFrame->setLayout(nameLayout);

    QFrame * hLine = new QFrame(this);
    hLine->setLineWidth(0);
    hLine->setFixedHeight(1);
    hLine->setStyleSheet("QFrame{background: rgba(0,0,0,0.2);}");

    m_layout->addWidget(iconLabel);
    m_layout->addWidget(nameFrame);
    m_layout->addWidget(hLine);

    //文件和文件夹有一个额外的详情区域
    if (type == SearchListView::ResType::Dir || type == SearchListView::ResType::File || type == SearchListView::ResType::Content) {
        QFrame * detailFrame = new QFrame(this);
        QVBoxLayout * detailLyt = new QVBoxLayout(detailFrame);
        detailLyt->setContentsMargins(0,0,0,0);
        if (type == SearchListView::ResType::Content) { //文件内容区域
            QLabel * contentLabel = new QLabel(detailFrame);
            contentLabel->setWordWrap(true);
            contentLabel->setContentsMargins(9, 0, 9, 0);
//            contentLabel->setText(m_contentText);
            contentLabel->setText(QApplication::translate("", getHtmlText(m_contentText, m_keyword).toLocal8Bit(), nullptr));
            detailLyt->addWidget(contentLabel);
        }
        QFrame * pathFrame = new QFrame(detailFrame);
        QFrame * timeFrame = new QFrame(detailFrame);
        QHBoxLayout * pathLyt = new QHBoxLayout(pathFrame);
        QHBoxLayout * timeLyt = new QHBoxLayout(timeFrame);
        QLabel * pathLabel_1 = new QLabel(pathFrame);
        QLabel * pathLabel_2 = new QLabel(pathFrame);
        pathLabel_1->setText(tr("Path"));
        pathLabel_2->setFixedWidth(240);
        pathLabel_2->setText(path);
        pathLabel_2->setWordWrap(true);
        pathLyt->addWidget(pathLabel_1);
        pathLyt->addStretch();
        pathLyt->addWidget(pathLabel_2);
        QLabel * timeLabel_1 = new QLabel(timeFrame);
        QLabel * timeLabel_2 = new QLabel(timeFrame);
        timeLabel_1->setText(tr("Last time modified"));
        QFileInfo fileInfo(path);
        timeLabel_2->setText(fileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss"));
        timeLyt->addWidget(timeLabel_1);
        timeLyt->addStretch();
        timeLyt->addWidget(timeLabel_2);
        detailLyt->addWidget(pathFrame);
        detailLyt->addWidget(timeFrame);

        QFrame * hLine_2 = new QFrame(this);
        hLine_2->setLineWidth(0);
        hLine_2->setFixedHeight(1);
        hLine_2->setStyleSheet("QFrame{background: rgba(0,0,0,0.2);}");

        m_layout->addWidget(detailFrame);
        m_layout->addWidget(hLine_2);
    }

    //可执行操作区域
    OptionView * optionView = new OptionView(this, type);
    connect(optionView, &OptionView::onOptionClicked, this, [ = ](const int& option) {
        execActions(type, option, path);
    });

    m_layout->addWidget(optionView);
    m_layout->addStretch();

    //根据不同类型的搜索结果切换加载图片和名称的方式
    switch (type) {
        case SearchListView::ResType::App : {
            QIcon icon = FileUtils::getAppIcon(path);
            iconLabel->setPixmap(icon.pixmap(icon.actualSize(QSize(96, 96))));
            QFontMetrics fontMetrics = nameLabel->fontMetrics();
            QString name = fontMetrics.elidedText(FileUtils::getAppName(path), Qt::ElideRight, 215); //当字体长度超过215时显示为省略号
            nameLabel->setText(name);
            typeLabel->setText(tr("Application"));
            break;
        }
        case SearchListView::ResType::Content:
        case SearchListView::ResType::Dir :
        case SearchListView::ResType::File : {
            QIcon icon = FileUtils::getFileIcon(QString("file://%1").arg(path));
            iconLabel->setPixmap(icon.pixmap(icon.actualSize(QSize(96, 96))));
            QFontMetrics fontMetrics = nameLabel->fontMetrics();
            QString name = fontMetrics.elidedText(FileUtils::getFileName(path), Qt::ElideRight, 215);
            nameLabel->setText(name);
            typeLabel->setText(tr("Document"));
            break;
        }
        case SearchListView::ResType::Setting : {
            QIcon icon = FileUtils::getSettingIcon(path, true);
            iconLabel->setPixmap(icon.pixmap(icon.actualSize(QSize(96, 96))));
            QString settingType = path.mid(path.indexOf("/") + 1, path.lastIndexOf("/") - path.indexOf("/") - 1); //配置项所属控制面板插件名
            nameLabel->setText(settingType);
            typeLabel->setText(FileUtils::getSettingName(path));
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
            g_app_info_launch(G_APP_INFO(desktopAppInfo),nullptr, nullptr, nullptr);
            g_object_unref(desktopAppInfo);
            return true;
            break;
        }
        case SearchListView::ResType::Content:
        case SearchListView::ResType::Dir:
        case SearchListView::ResType::File: {
            QProcess process;
            process.start(QString("xdg-open %1").arg(path));
            return true;
            break;
        }
        case SearchListView::ResType::Setting: {
            //打开控制面板对应页面
            QProcess process;
            process.start(QString("ukui-control-center --%1").arg(path.left(path.indexOf("/")).toLower()));
            return true;
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
        process.start(QString("chmod a+x %1").arg(newName));
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
    QProcess process;
    process.start(QString("xdg-open %1").arg(path.left(path.lastIndexOf("/"))));
    return true;
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
