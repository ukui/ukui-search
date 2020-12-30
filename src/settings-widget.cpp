#include "settings-widget.h"
#include <QPainter>
#include <QPainterPath>

extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);
SettingsWidget::SettingsWidget(QWidget *parent) : QWidget(parent)
{
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    initUi();
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
 * @brief SettingsWidget::onBtnConfirmClicked 点击确认按钮的槽函数
 */
void SettingsWidget::onBtnConfirmClicked() {

}

/**
 * @brief SettingsWidget::onBtnCancelClicked 点击取消按钮的槽函数
 */
void SettingsWidget::onBtnCancelClicked() {
    this->close();
}

/**
 * @brief SettingsWidget::onBtnAddClicked 点击添加黑名单按钮的槽函数
 */
void SettingsWidget::onBtnAddClicked() {

}

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
