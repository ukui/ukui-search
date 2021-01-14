#include "settings-widget.h"
#include <QPainter>
#include <QPainterPath>
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include "folder-list-item.h"
#include "global-settings.h"
#include "file-utils.h"

extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);
SettingsWidget::SettingsWidget(QWidget *parent) : QWidget(parent)
{
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    initUi();
    refreshIndexState();
    setupBlackList(GlobalSettings::getInstance()->getBlockDirs());
}

SettingsWidget::~SettingsWidget()
{
}

/**
 * @brief SettingsWidget::initUi 初始化界面UI
 */
void SettingsWidget::initUi() {
    this->setFixedWidth(528);
    this->setMinimumHeight(460);
    this->setMaximumHeight(680);
    m_mainLyt = new QVBoxLayout(this);
    m_mainLyt->setContentsMargins(24, 9, 24, 24);
    this->setLayout(m_mainLyt);
    this->setStyleSheet("QLabel{color: palette(text);}");

    //标题栏
    m_titleFrame = new QFrame(this);
    m_titleFrame->setFixedHeight(40);
    m_titleLyt = new QHBoxLayout(m_titleFrame);
    m_titleLyt->setContentsMargins(0, 0, 0, 0);
    m_titleFrame->setLayout(m_titleLyt);
    m_titleIcon = new QLabel(m_titleFrame);
    m_titleIcon->setPixmap(QPixmap(":/res/icons/edit-find-symbolic.svg"));
    m_titleLabel = new QLabel(m_titleFrame);
    m_titleLabel->setText(tr("Search"));
    m_closeBtn = new QPushButton(m_titleFrame);
    m_closeBtn->setFixedSize(24, 24);
    m_closeBtn->setIcon(QIcon(":/res/icons/close.svg"));
    m_closeBtn->setStyleSheet("QPushButton{background: transparent;}"
                              "QPushButton:hover:!pressed{background: transparent;}");
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

    //设置
    m_settingLabel = new QLabel(this);
    m_settingLabel->setText(tr("Settings"));
    m_settingLabel->setStyleSheet("QLabel{font-size: 24px; color: palette(text);}");
    m_mainLyt->addWidget(m_settingLabel);

    //文件索引
    m_indexTitleLabel = new QLabel(this);
    m_indexTitleLabel->setText(tr("Index State"));
    m_indexTitleLabel->setStyleSheet("QLabel{font-size: 16px; font-weight:bold; color: palette(text);}");
    m_indexStateLabel = new QLabel(this);
    m_indexStateLabel->setText(tr("..."));
    m_indexNumLabel = new QLabel(this);
    m_indexNumLabel->setText(tr("..."));
    m_mainLyt->addWidget(m_indexTitleLabel);
    m_mainLyt->addWidget(m_indexStateLabel);
    m_mainLyt->addWidget(m_indexNumLabel);

    //文件索引设置（黑名单）
    m_indexSettingLabel = new QLabel(this);
    m_indexSettingLabel->setText(tr("File Index Settings"));
    m_indexSettingLabel->setStyleSheet("QLabel{font-size: 16px; font-weight:bold; color: palette(text);}");
    m_indexDescLabel = new QLabel(this);
    m_indexDescLabel->setText(tr("Following folders will not be searched. You can set it by adding and removing folders."));
    m_indexDescLabel->setWordWrap(true);
    m_indexBtnFrame = new QFrame(this);
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
    m_dirListArea = new QScrollArea(this);
    m_dirListArea->setStyleSheet("QScrollArea{background:transparent;}");
    m_dirListWidget = new QWidget(this);
    m_dirListWidget->setStyleSheet("QWidget{background:transparent;}");
    m_dirListLyt = new QVBoxLayout(m_dirListWidget);
    m_dirListLyt->setContentsMargins(0, 0, 0, 0);
    m_dirListLyt->setSpacing(0);
    m_dirListWidget->setLayout(m_dirListLyt);
    m_dirListArea->setWidget(m_dirListWidget);
    m_dirListArea->setWidgetResizable(true);
    m_mainLyt->addWidget(m_indexSettingLabel);
    m_mainLyt->addWidget(m_indexDescLabel);
    m_mainLyt->addWidget(m_indexBtnFrame);
    m_mainLyt->addWidget(m_dirListArea);

    //搜索引擎设置
    m_searchEngineLabel = new QLabel(this);
    m_searchEngineLabel->setText(tr("Search Engine Settings"));
    m_searchEngineLabel->setStyleSheet("QLabel{font-size: 16px; font-weight:bold; color: palette(text);}");
    m_engineDescLabel = new QLabel(this);
    m_engineDescLabel->setText(tr("Please select search engine you preferred."));
    m_engineDescLabel->setWordWrap(true);
    m_engineBtnGroup = new QButtonGroup(this);
    m_baiduBtn = new QRadioButton(this);
    m_sougouBtn = new QRadioButton(this);
    m_360Btn = new QRadioButton(this);
    m_baiduBtn->setFixedSize(16, 16);
    m_sougouBtn->setFixedSize(16, 16);
    m_360Btn->setFixedSize(16, 16);
    m_radioBtnFrame = new QFrame(this);
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
    m_mainLyt->addWidget(m_searchEngineLabel);
    m_mainLyt->addWidget(m_engineDescLabel);
    m_mainLyt->addWidget(m_radioBtnFrame);

    //取消与确认按钮
    m_bottomBtnFrame = new QFrame(this);
    m_bottomBtnLyt = new QHBoxLayout(m_bottomBtnFrame);
    m_bottomBtnFrame->setLayout(m_bottomBtnLyt);
    m_bottomBtnLyt->setSpacing(20);
    m_cancelBtn = new QPushButton(m_bottomBtnFrame);
    m_cancelBtn->setText(tr("Cancel"));
    m_cancelBtn->setFixedSize(80, 32);
    connect(m_cancelBtn, &QPushButton::clicked, this, &SettingsWidget::onBtnCancelClicked);
    m_confirmBtn = new QPushButton(m_bottomBtnFrame);
    m_confirmBtn->setText(tr("Confirm"));
    m_confirmBtn->setFixedSize(80, 32);
    connect(m_confirmBtn, &QPushButton::clicked, this, &SettingsWidget::onBtnConfirmClicked);
    m_bottomBtnLyt->addStretch();
    m_bottomBtnLyt->addWidget(m_cancelBtn);
    m_bottomBtnLyt->addWidget(m_confirmBtn);
    m_mainLyt->addWidget(m_bottomBtnFrame);

    m_mainLyt->addStretch();
}

/**
 * @brief SettingsWidget::setupBlackList 创建黑名单列表
 * @param list 文件夹路径列表
 */
void SettingsWidget::setupBlackList(const QStringList& list) {
    clearLayout(m_dirListLyt);
    Q_FOREACH(QString path, list) {
        FolderListItem * item = new FolderListItem(m_dirListWidget, path);
        m_dirListLyt->addWidget(item);
        item->setMaximumWidth(470);
        //测试用，实际调用中应等待后端完成操作后删除该控件
        connect(item, SIGNAL(onDelBtnClicked(const QString&)), this, SLOT(onBtnDelClicked(const QString&)));
    }
    m_dirListLyt->addStretch();
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
    m_indexStateLabel->setText(QString::number(FileUtils::_index_status));
    m_indexNumLabel->setText(QString("%1/%2").arg(QString::number(FileUtils::_current_index_count)).arg(QString::number(FileUtils::_max_index_count)));
    m_timer = new QTimer;
    connect(m_timer, &QTimer::timeout, this, [ = ]() {
        m_indexStateLabel->setText(QString::number(FileUtils::_index_status));
        m_indexNumLabel->setText(QString("%1/%2").arg(QString::number(FileUtils::_current_index_count)).arg(QString::number(FileUtils::_max_index_count)));
    });
    m_timer->start(0.5 * 1000);
}

/**
 * @brief SettingsWidget::onBtnDelClicked 删除黑名单中的目录
 * @param path 文件夹路径
 */
void SettingsWidget::onBtnDelClicked(const QString& path) {
    QString returnMessage;
    if (GlobalSettings::getInstance()->setBlockDirs(path, returnMessage, true)) {
        qDebug()<<"Remove block dir in onBtnDelClicked() successed.";
        Q_FOREACH (FolderListItem * item, m_dirListWidget->findChildren<FolderListItem*>()) {
            if (item->getPath() == path) {
                item->deleteLater();
                item = NULL;
                return;
            }
        }

    } else {
        qWarning()<<returnMessage;
        qDebug()<<"Remove block dir in onBtnAddClicked() failed. Message: "<<returnMessage;
    }
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
    this->show();
}

/**
 * @brief SettingsWidget::onBtnConfirmClicked 点击确认按钮的槽函数
 */
void SettingsWidget::onBtnConfirmClicked() {
    Q_EMIT this->settingWidgetClosed();
    m_timer->stop();
    this->close();
}

/**
 * @brief SettingsWidget::onBtnCancelClicked 点击取消按钮的槽函数
 */
void SettingsWidget::onBtnCancelClicked() {
    Q_EMIT this->settingWidgetClosed();
    m_timer->stop();
    this->close();
}

/**
 * @brief SettingsWidget::onBtnAddClicked 点击添加黑名单按钮的槽函数
 */
void SettingsWidget::onBtnAddClicked() {
    QFileDialog * fileDialog = new QFileDialog(this);
//    fileDialog->setFileMode(QFileDialog::Directory); //允许查看文件和文件夹，但只允许选择文件夹
    fileDialog->setFileMode(QFileDialog::DirectoryOnly); //只允许查看文件夹
    fileDialog->setViewMode(QFileDialog::Detail);
    fileDialog->setDirectory(QDir::homePath());
    if (fileDialog->exec() != QDialog::Accepted) {
        fileDialog->deleteLater();
        return;
    }
    QString selectedDir = 0;
    QString returnMessage = 0;
    selectedDir = fileDialog->selectedFiles().first();
    qDebug()<<"Selected a folder in onBtnAddClicked(): "<<selectedDir<<". ->settings-widget.cpp #238";
    if (GlobalSettings::getInstance()->setBlockDirs(selectedDir, returnMessage)) {
        setupBlackList(GlobalSettings::getInstance()->getBlockDirs());
        qDebug()<<"Add block dir in onBtnAddClicked() successed. ->settings-widget.cpp #238";
    } else {
        qWarning()<<returnMessage;
        qDebug()<<"Add block dir in onBtnAddClicked() failed. Message: "<<returnMessage<<" ->settings-widget.cpp #238";
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
    rectPath.addRoundedRect(this->rect().adjusted(10, 10, -10, -10), 6, 6);

    // 画一个黑底
    QPixmap pixmap(this->rect().size());
    pixmap.fill(Qt::transparent);
    QPainter pixmapPainter(&pixmap);
    pixmapPainter.setRenderHint(QPainter::Antialiasing);
    pixmapPainter.setPen(Qt::transparent);
    pixmapPainter.setBrush(Qt::black);
    pixmapPainter.setOpacity(0.65);
    pixmapPainter.drawPath(rectPath);
    pixmapPainter.end();

    // 模糊这个黑底
    QImage img = pixmap.toImage();
    qt_blurImage(img, 10, false, false);

    // 挖掉中心
    pixmap = QPixmap::fromImage(img);
    QPainter pixmapPainter2(&pixmap);
    pixmapPainter2.setRenderHint(QPainter::Antialiasing);
    pixmapPainter2.setCompositionMode(QPainter::CompositionMode_Clear);
    pixmapPainter2.setPen(Qt::transparent);
    pixmapPainter2.setBrush(Qt::transparent);
    pixmapPainter2.drawPath(rectPath);

    // 绘制阴影
    p.drawPixmap(this->rect(), pixmap, pixmap.rect());

    // 绘制一个背景
    p.save();
    p.fillPath(rectPath,palette().color(QPalette::Base));
    p.restore();

}
