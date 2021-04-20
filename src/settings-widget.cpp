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
#include "settings-widget.h"
#include <QPainter>
#include <QPainterPath>
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include "folder-list-item.h"
#include "global-settings.h"
#include "file-utils.h"

extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);
SettingsWidget::SettingsWidget(QWidget *parent) : QWidget(parent)
{
    this->setWindowIcon(QIcon::fromTheme("kylin-search"));
    this->setWindowTitle(tr("ukui-search-settings"));
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
//    this->setAttribute(Qt::WA_TranslucentBackground);

//    m_hints.flags = MWM_HINTS_FUNCTIONS|MWM_HINTS_DECORATIONS;
//    m_hints.functions = MWM_FUNC_ALL;
//    m_hints.decorations = MWM_DECOR_BORDER;
//    XAtomHelper::getInstance()->setWindowMotifHint(winId(), m_hints);

    initUi();
    refreshIndexState();
    setupBlackList(GlobalSettings::getInstance()->getBlockDirs());
    resetWebEngine();
}

SettingsWidget::~SettingsWidget()
{
}

/**
 * @brief SettingsWidget::initUi 初始化界面UI
 */
void SettingsWidget::initUi() {
//    this->setFixedWidth(528);
//    this->setMinimumHeight(460);
//    this->setMaximumHeight(680);
    m_mainLyt = new QVBoxLayout(this);
    m_mainLyt->setContentsMargins(16, 8, 16, 24);
    this->setLayout(m_mainLyt);

    //标题栏
    m_titleFrame = new QFrame(this);
    m_titleFrame->setFixedHeight(40);
    m_titleLyt = new QHBoxLayout(m_titleFrame);
    m_titleLyt->setContentsMargins(0, 0, 0, 0);
    m_titleFrame->setLayout(m_titleLyt);
    m_titleIcon = new QLabel(m_titleFrame);
    m_titleIcon->setPixmap(QIcon::fromTheme("kylin-search").pixmap(QSize(24, 24)));
    m_titleLabel = new QLabel(m_titleFrame);
    m_titleLabel->setText(tr("Search"));
    m_closeBtn = new QPushButton(m_titleFrame);
    m_closeBtn->setFixedSize(24, 24);
//    m_closeBtn->setIcon(QIcon(":/res/icons/close.svg"));
//    m_closeBtn->setStyleSheet("QPushButton{background: transparent;}"
//                              "QPushButton:hover:!pressed{background: transparent;}");
    m_closeBtn->setIcon(QIcon::fromTheme("window-close-symbolic"));
    m_closeBtn->setProperty("isWindowButton", 0x02);
    m_closeBtn->setProperty("useIconHighlightEffect", 0x08);
    m_closeBtn->setFlat(true);
    connect(m_closeBtn, &QPushButton::clicked, this, [ = ]() {
        Q_EMIT this->settingWidgetClosed();
        m_timer->stop();
        this->close();
    });
    m_titleLyt->addWidget(m_titleIcon);
    m_titleLyt->addWidget(m_titleLabel);
    m_titleLyt->addStretch();
    m_titleLyt->addWidget(m_closeBtn);
    m_mainLyt->addWidget(m_titleFrame);

    m_contentFrame = new QFrame(this);
    m_contentLyt = new QVBoxLayout(m_contentFrame);
    m_contentFrame->setLayout(m_contentLyt);
    m_contentLyt->setContentsMargins(8,0,8,0);
    m_mainLyt->addWidget(m_contentFrame);

    //设置
    m_settingLabel = new QLabel(m_contentFrame);
    m_settingLabel->setText(tr("<h2>Settings</h2>"));
    m_contentLyt->addWidget(m_settingLabel);

    //文件索引
    m_indexTitleLabel = new QLabel(m_contentFrame);
    m_indexTitleLabel->setText(tr("<h3>Index State</h3>"));
    m_indexStateLabel = new QLabel(m_contentFrame);
    m_indexStateLabel->setText(tr("..."));
    m_indexNumLabel = new QLabel(m_contentFrame);
    m_indexNumLabel->setText(tr("..."));
    m_contentLyt->addWidget(m_indexTitleLabel);
    m_contentLyt->addWidget(m_indexStateLabel);
//    m_mainLyt->addWidget(m_indexNumLabel);
    m_indexNumLabel->hide();

    //文件索引设置（黑名单）
    m_indexSettingLabel = new QLabel(m_contentFrame);
    m_indexSettingLabel->setText(tr("<h3>File Index Settings</h3>"));
    m_indexDescLabel = new QLabel(m_contentFrame);
    m_indexDescLabel->setText(tr("Following folders will not be searched. You can set it by adding and removing folders."));
    m_indexDescLabel->setWordWrap(true);
    m_indexBtnFrame = new QFrame(m_contentFrame);
    m_indexBtnLyt = new QHBoxLayout(m_indexBtnFrame);
    m_indexBtnLyt->setContentsMargins(0, 0, 0, 0);
    m_indexBtnFrame->setLayout(m_indexBtnLyt);
    m_addDirBtn = new QPushButton(m_indexBtnFrame);
    m_addDirBtn->setFixedHeight(32);
    m_addDirBtn->setMinimumWidth(164);
    m_addDirBtn->setText(tr("Add ignored folders"));
    connect(m_addDirBtn, &QPushButton::clicked, this, &SettingsWidget::onBtnAddClicked);
    m_indexBtnLyt->addWidget(m_addDirBtn);
    m_indexBtnLyt->addStretch();
    m_dirListArea = new QScrollArea(m_contentFrame);
    m_dirListArea->setStyleSheet("QScrollArea{background:transparent;}");
    m_dirListWidget = new QWidget(m_contentFrame);
    m_dirListWidget->setStyleSheet("QWidget{background:transparent;}");
    m_dirListLyt = new QVBoxLayout(m_dirListWidget);
    m_dirListLyt->setContentsMargins(0, 0, 0, 0);
    m_dirListLyt->setSpacing(0);
    m_dirListWidget->setLayout(m_dirListLyt);
    m_dirListArea->setWidget(m_dirListWidget);
    m_dirListArea->setWidgetResizable(m_contentFrame);
    m_contentLyt->addWidget(m_indexSettingLabel);
    m_contentLyt->addWidget(m_indexDescLabel);
    m_contentLyt->addWidget(m_indexBtnFrame);
    m_contentLyt->addWidget(m_dirListArea);

    //搜索引擎设置
    m_searchEngineLabel = new QLabel(m_contentFrame);
    m_searchEngineLabel->setText(tr("<h3>Search Engine Settings</h3>"));
    m_engineDescLabel = new QLabel(m_contentFrame);
    m_engineDescLabel->setText(tr("Please select search engine you preferred."));
    m_engineDescLabel->setWordWrap(true);
    m_engineBtnGroup = new QButtonGroup(m_contentFrame);
    m_baiduBtn = new QRadioButton(m_contentFrame);
    m_sougouBtn = new QRadioButton(m_contentFrame);
    m_360Btn = new QRadioButton(m_contentFrame);
    m_baiduBtn->setFixedSize(16, 16);
    m_sougouBtn->setFixedSize(16, 16);
    m_360Btn->setFixedSize(16, 16);
    m_radioBtnFrame = new QFrame(m_contentFrame);
    m_radioBtnLyt = new QHBoxLayout(m_radioBtnFrame);
    m_radioBtnFrame->setLayout(m_radioBtnLyt);
    m_baiduLabel = new QLabel();
    m_baiduLabel->setText(tr("baidu"));
    m_sougouLabel = new QLabel();
    m_sougouLabel->setText(tr("sougou"));
    m_360Label = new QLabel();
    m_360Label->setText(tr("360"));
    m_radioBtnLyt->setContentsMargins(0, 0, 0, 0);
    m_radioBtnLyt->addWidget(m_baiduBtn);
    m_radioBtnLyt->addWidget(m_baiduLabel);
    m_radioBtnLyt->addWidget(m_sougouBtn);
    m_radioBtnLyt->addWidget(m_sougouLabel);
    m_radioBtnLyt->addWidget(m_360Btn);
    m_radioBtnLyt->addWidget(m_360Label);
    m_radioBtnLyt->addStretch();
    m_engineBtnGroup->setExclusive(true);
    m_engineBtnGroup->addButton(m_baiduBtn);
    m_engineBtnGroup->addButton(m_sougouBtn);
    m_engineBtnGroup->addButton(m_360Btn);
//    m_engineBtnGroup->setId(m_baiduBtn, WebEngine::Baidu);
//    m_engineBtnGroup->setId(m_sougouBtn, WebEngine::Sougou);
//    m_engineBtnGroup->setId(m_360Btn, WebEngine::_360);
//    connect(m_engineBtnGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [ = ] (int id) {
//        setWebEngine(id);
//    });
    connect(m_baiduBtn, &QRadioButton::clicked, [ = ] (bool checked) {
        if (checked) setWebEngine("baidu");
    });
    connect(m_sougouBtn, &QRadioButton::clicked, [ = ] (bool checked) {
        if (checked) setWebEngine("sougou");
    });
    connect(m_360Btn, &QRadioButton::clicked, [ = ] (bool checked) {
        if (checked) setWebEngine("360");
    });

    m_contentLyt->addWidget(m_searchEngineLabel);
    m_contentLyt->addWidget(m_engineDescLabel);
    m_contentLyt->addWidget(m_radioBtnFrame);

    //取消与确认按钮 （隐藏）
//    m_bottomBtnFrame = new QFrame(this);
//    m_bottomBtnLyt = new QHBoxLayout(m_bottomBtnFrame);
//    m_bottomBtnFrame->setLayout(m_bottomBtnLyt);
//    m_bottomBtnLyt->setSpacing(20);
//    m_cancelBtn = new QPushButton(m_bottomBtnFrame);
//    m_cancelBtn->setText(tr("Cancel"));
//    m_cancelBtn->setFixedSize(80, 32);
//    connect(m_cancelBtn, &QPushButton::clicked, this, &SettingsWidget::onBtnCancelClicked);
//    m_confirmBtn = new QPushButton(m_bottomBtnFrame);
//    m_confirmBtn->setText(tr("Confirm"));
//    m_confirmBtn->setFixedSize(80, 32);
//    connect(m_confirmBtn, &QPushButton::clicked, this, &SettingsWidget::onBtnConfirmClicked);
//    m_bottomBtnLyt->addStretch();
//    m_bottomBtnLyt->addWidget(m_cancelBtn);
//    m_bottomBtnLyt->addWidget(m_confirmBtn);
//    m_mainLyt->addWidget(m_bottomBtnFrame);

    m_contentLyt->addStretch();
}

/**
 * @brief SettingsWidget::setupBlackList 创建黑名单列表
 * @param list 文件夹路径列表
 */
void SettingsWidget::setupBlackList(const QStringList& list) {
    clearLayout(m_dirListLyt);
    m_blockdirs = 0;
    Q_FOREACH(QString path, list) {
        FolderListItem * item = new FolderListItem(m_dirListWidget, path);
        m_dirListLyt->addWidget(item);
        item->setMaximumWidth(this->width() - 52);
        connect(item, &FolderListItem::onDelBtnClicked, this, &SettingsWidget::onBtnDelClicked);
        m_blockdirs ++;
    }
    this->resize();
    m_dirListWidget->setFixedWidth(this->width() - 68);
//    m_dirListLyt->addStretch();
}

/**
 * @brief SettingsWidget::clearLayout 清空某个布局
 * @param layout 需要清空的布局
 */
void SettingsWidget::clearLayout(QLayout * layout) {
    if (! layout) return;
    QLayoutItem * child;
    while ((child = layout->takeAt(0)) != 0) {
        if(child->widget())
        {
            child->widget()->setParent(NULL);
        }
        delete child;
    }
    child = NULL;
}

/**
 * @brief SettingsWidget::refreshIndexState 定时刷新索引项
 */
void SettingsWidget::refreshIndexState()
{
//    qDebug()<<"FileUtils::_index_status: "<<FileUtils::_index_status;
    if (FileUtils::_index_status != 0) {
        this->setIndexState(true);
    } else {
        this->setIndexState(false);
    }
    m_indexNumLabel->setText(QString("%1/%2").arg(QString::number(SearchManager::getCurrentIndexCount())).arg(QString::number(FileUtils::_max_index_count)));
    m_timer = new QTimer;
    connect(m_timer, &QTimer::timeout, this, [ = ]() {
        qDebug()<<"FileUtils::_index_status: "<<FileUtils::_index_status;
        if (FileUtils::_index_status != 0) {
            this->setIndexState(true);
        } else {
            this->setIndexState(false);
        }
        m_indexNumLabel->setText(QString("%1/%2").arg(QString::number(SearchManager::getCurrentIndexCount())).arg(QString::number(FileUtils::_max_index_count)));
    });
    m_timer->start(0.5 * 1000);
}

/**
 * @brief SettingsWidget::onBtnDelClicked 删除黑名单中的目录
 * @param path 文件夹路径
 */
void SettingsWidget::onBtnDelClicked(const QString& path) {
    QMessageBox message(QMessageBox::Question, tr("Search"), tr("Whether to delete this directory?"));
    QPushButton * buttonYes = message.addButton(tr("Yes"), QMessageBox::YesRole);
    message.addButton(tr("No"), QMessageBox::NoRole);
    message.exec();
    if (message.clickedButton() != buttonYes) {
        return;
    }

    int returnCode = 0;
    if (GlobalSettings::getInstance()->setBlockDirs(path, returnCode, true)) {
        qDebug()<<"Remove block dir in onBtnDelClicked() successed.";
        Q_FOREACH (FolderListItem * item, m_dirListWidget->findChildren<FolderListItem*>()) {
            if (item->getPath() == path) {
                item->deleteLater();
                item = NULL;
                m_blockdirs --;
                this->resize();
                return;
            }
        }
    } else {
        showWarningDialog(returnCode);
    }
}

/**
 * @brief SettingsWidget::resetWebEngine 获取当前的搜索引擎并反应在UI控件上
 */
void SettingsWidget::resetWebEngine()
{
    QString engine = GlobalSettings::getInstance()->getValue(WEB_ENGINE).toString();
    m_engineBtnGroup->blockSignals(true);
    if (!engine.isEmpty()) {
        if (engine == "360") {
            m_360Btn->setChecked(true);
        } else if (engine == "sougou") {
            m_sougouBtn->setChecked(true);
        } else {
            m_baiduBtn->setChecked(true);
        }
    } else {
        m_baiduBtn->setChecked(true);
    }
    m_engineBtnGroup->blockSignals(false);
}

/**
 * @brief SettingsWidget::setWebEngine
 * @param engine 选择的搜索引擎
 */
void SettingsWidget::setWebEngine(const QString& engine)
{
    GlobalSettings::getInstance()->setValue(WEB_ENGINE, engine);
}

/**
 * @brief setIndexState 设置当前索引状态
 * @param isCreatingIndex 是否正在创建索引
 */
void SettingsWidget::setIndexState(bool isCreatingIndex) {
    if (isCreatingIndex) {
        m_indexStateLabel->setText(tr("Creating ..."));
        return;
    }
    m_indexStateLabel->setText(tr("Done"));
}

/**
 * @brief SettingsWidget::setIndexNum 设置当前索引项数量
 * @param num 索引项数量
 */
void SettingsWidget::setIndexNum(int num) {
    m_indexNumLabel->setText(QString(tr("Index Entry: %1")).arg(QString::number(num)));
}

/**
 * @brief SettingsWidget::showWidget 显示此窗口
 */
void SettingsWidget::showWidget()
{
    Qt::WindowFlags flags = this->windowFlags();
    flags |= Qt::WindowStaysOnTopHint;
    this->setWindowFlags(flags);
    flags &= ~Qt::WindowStaysOnTopHint;
    this->setWindowFlags(flags);
    m_timer->start();
//    XAtomHelper::getInstance()->setWindowMotifHint(winId(), m_hints);
    this->show();
}

///**
// * @brief SettingsWidget::onBtnConfirmClicked 点击确认按钮的槽函数
// */
//void SettingsWidget::onBtnConfirmClicked() {
//    Q_EMIT this->settingWidgetClosed();
//    m_timer->stop();
//    this->close();
//}

///**
// * @brief SettingsWidget::onBtnCancelClicked 点击取消按钮的槽函数
// */
//void SettingsWidget::onBtnCancelClicked() {
//    Q_EMIT this->settingWidgetClosed();
//    m_timer->stop();
//    this->close();
//}

/**
 * @brief SettingsWidget::onBtnAddClicked 点击添加黑名单按钮的槽函数
 */
void SettingsWidget::onBtnAddClicked() {
    QFileDialog * fileDialog = new QFileDialog(this);
//    fileDialog->setFileMode(QFileDialog::Directory); //允许查看文件和文件夹，但只允许选择文件夹
    fileDialog->setFileMode(QFileDialog::DirectoryOnly); //只允许查看文件夹
//    fileDialog->setViewMode(QFileDialog::Detail);
    fileDialog->setDirectory(QDir::homePath());
    fileDialog->setNameFilter(tr("Directories"));
    fileDialog->setWindowTitle(tr("select blocked folder"));
    fileDialog->setLabelText(QFileDialog::Accept, tr("Select"));
    fileDialog->setLabelText(QFileDialog::LookIn, tr("Position: "));
    fileDialog->setLabelText(QFileDialog::FileName, tr("FileName: "));
    fileDialog->setLabelText(QFileDialog::FileType, tr("FileType: "));
    fileDialog->setLabelText(QFileDialog::Reject, tr("Cancel"));
    if (fileDialog->exec() != QDialog::Accepted) {
        fileDialog->deleteLater();
        return;
    }
    QString selectedDir = 0;
    int returnCode;
    selectedDir = fileDialog->selectedFiles().first();
    qDebug()<<"Selected a folder in onBtnAddClicked(): "<<selectedDir<<". ->settings-widget.cpp #238";
    if (GlobalSettings::getInstance()->setBlockDirs(selectedDir, returnCode)) {
        setupBlackList(GlobalSettings::getInstance()->getBlockDirs());
        qDebug()<<"Add block dir in onBtnAddClicked() successed. ->settings-widget.cpp #238";
    } else {
        showWarningDialog(returnCode);
    }
}

/**
 * @brief SettingsWidget::paintEvent 绘制弹窗阴影
 * @param event
 */
void SettingsWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    QPainterPath rectPath;
//    rectPath.addRoundedRect(this->rect(), 6, 6);
    rectPath.addRect(this->rect());

//    // 画一个黑底
//    QPixmap pixmap(this->rect().size());
//    pixmap.fill(Qt::transparent);
//    QPainter pixmapPainter(&pixmap);
//    pixmapPainter.setRenderHint(QPainter::Antialiasing);
//    pixmapPainter.setPen(Qt::transparent);
//    pixmapPainter.setBrush(Qt::black);
//    pixmapPainter.setOpacity(0.65);
//    pixmapPainter.drawPath(rectPath);
//    pixmapPainter.end();

//    // 模糊这个黑底
//    QImage img = pixmap.toImage();
//    qt_blurImage(img, 10, false, false);

//    // 挖掉中心
//    pixmap = QPixmap::fromImage(img);
//    QPainter pixmapPainter2(&pixmap);
//    pixmapPainter2.setRenderHint(QPainter::Antialiasing);
//    pixmapPainter2.setCompositionMode(QPainter::CompositionMode_Clear);
//    pixmapPainter2.setPen(Qt::transparent);
//    pixmapPainter2.setBrush(Qt::transparent);
//    pixmapPainter2.drawPath(rectPath);

//    // 绘制阴影
//    p.drawPixmap(this->rect(), pixmap, pixmap.rect());

    // 绘制一个背景
    p.save();
    p.fillPath(rectPath,palette().color(QPalette::Base));
    p.restore();
}

/**
 * @brief SettingsWidget::resize 重新计算窗口应有大小
 */
void SettingsWidget::resize()
{
//    if (m_blockdirs <= 1) {
//        this->setFixedSize(528, 455);
//    } else if (m_blockdirs <= 3) {
//        this->setFixedSize(528, 425 + 30 * m_blockdirs);
//    } else {
//        this->setFixedSize(528, 515);
//    }
    if (m_blockdirs <= 4) {
        m_dirListArea->setFixedHeight(32 * m_blockdirs + 4);
        m_dirListWidget->setFixedHeight(32 * m_blockdirs);
    } else {
        m_dirListWidget->setFixedHeight(32 * m_blockdirs);
        m_dirListArea->setFixedHeight(32 * 4 + 4);
    }
    this->setFixedSize(528, 410 + m_dirListArea->height());
}

/**
 * @brief SettingsWidget::showWarningDialog 显示警告弹窗
 * @param errorCode 错误码
 */
void SettingsWidget::showWarningDialog(const int & errorCode)
{
    qWarning()<<"Add block dir in onBtnAddClicked() failed. Code: "<<errorCode<<" ->settings-widget.cpp #238";
    QString errorMessage;
    switch (errorCode) {
        case 1: {
            errorMessage = tr("Choosen path is Empty!");
            break;
        }
        case 2: {
            errorMessage = tr("Choosen path is not in \"home\"!");
            break;
        }
        case 3: {
            errorMessage = tr("Its' parent folder has been blocked!");
            break;
        }
        default: {
            errorMessage = tr("Set blocked folder failed!");
            break;
        }
    }
    QMessageBox message(QMessageBox::Warning, tr("Search"), errorMessage);
    message.addButton(tr("OK"), QMessageBox::AcceptRole);
    message.exec();
}
