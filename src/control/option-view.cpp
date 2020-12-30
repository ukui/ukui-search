#include "option-view.h"
#include <QDebug>
#include <QEvent>

OptionView::OptionView(QWidget *parent, const int& type) : QWidget(parent)
{
    m_mainLyt = new QVBoxLayout(this);
    this->setLayout(m_mainLyt);
    m_mainLyt->setContentsMargins(0,8,0,0);
    m_mainLyt->setSpacing(8);
    initComponent(type);
}

OptionView::~OptionView()
{
    if (m_openLabel) {
        delete m_openLabel;
        m_openLabel = NULL;
    }
    if (m_shortcutLabel) {
        delete m_shortcutLabel;
        m_shortcutLabel = NULL;
    }
    if (m_panelLabel) {
        delete m_panelLabel;
        m_panelLabel = NULL;
    }
    if (m_openPathLabel) {
        delete m_openPathLabel;
        m_openPathLabel = NULL;
    }
    if (m_copyPathLabel) {
        delete m_copyPathLabel;
        m_copyPathLabel = NULL;
    }
}

/**
 * @brief OptionView::initComponent 构建可用选项表
 * @param type 详情页类型
 */
void OptionView::initComponent(const int& type) {
    switch (type) {
        case SearchListView::ResType::App : {
            setupAppOptions();
            break;
        }
        case SearchListView::ResType::File : {
            setupFileOptions();
            break;
        }
        case SearchListView::ResType::Setting : {
            setupSettingOptions();
            break;
        }
        case SearchListView::ResType::Dir : {
            setupDirOptions();
            break;
        }
        default:
            break;
    }
}

/**
 * @brief setupOptionLabel 创建每一个单独的选项
 * @param opt 选项类型
 */
void OptionView::setupOptionLabel(const int& opt) {
    QFrame * optionFrame = new QFrame(this);
    QHBoxLayout * optionLyt = new QHBoxLayout(optionFrame);
    optionLyt->setContentsMargins(8, 0, 0, 0);
    switch (opt) {
        case Options::Open: {
            m_openLabel = new QLabel(optionFrame);
            m_openLabel->setText(tr("Open")); //打开
            m_openLabel->setStyleSheet("QLabel{font-size: 14px; color: #3D6BE5}");
            m_openLabel->setCursor(QCursor(Qt::PointingHandCursor));
            m_openLabel->installEventFilter(this);
            optionLyt->addWidget(m_openLabel);
            break;
        }
        case Options::Shortcut: {
            m_shortcutLabel = new QLabel(optionFrame);
            m_shortcutLabel->setText(tr("Add Shortcut to Desktop")); //添加到桌面快捷方式
            m_shortcutLabel->setStyleSheet("QLabel{font-size: 14px; color: #3D6BE5}");
            m_shortcutLabel->setCursor(QCursor(Qt::PointingHandCursor));
            m_shortcutLabel->installEventFilter(this);
            optionLyt->addWidget(m_shortcutLabel);
            break;
        }
        case Options::Panel: {
            m_panelLabel = new QLabel(optionFrame);
            m_panelLabel->setText(tr("Add Shortcut to Panel")); //添加到任务栏快捷方式
            m_panelLabel->setStyleSheet("QLabel{font-size: 14px; color: #3D6BE5}");
            m_panelLabel->setCursor(QCursor(Qt::PointingHandCursor));
            m_panelLabel->installEventFilter(this);
            optionLyt->addWidget(m_panelLabel);
            break;
        }
        case Options::OpenPath: {
            m_openPathLabel = new QLabel(optionFrame);
            m_openPathLabel->setText(tr("Open path")); //打开所在路径
            m_openPathLabel->setStyleSheet("QLabel{font-size: 14px; color: #3D6BE5}");
            m_openPathLabel->setCursor(QCursor(Qt::PointingHandCursor));
            m_openPathLabel->installEventFilter(this);
            optionLyt->addWidget(m_openPathLabel);
            break;
        }
        case Options::CopyPath: {
            m_copyPathLabel = new QLabel(optionFrame);
            m_copyPathLabel->setText(tr("Copy path")); //复制所在路径
            m_copyPathLabel->setStyleSheet("QLabel{font-size: 14px; color: #3D6BE5}");
            m_copyPathLabel->setCursor(QCursor(Qt::PointingHandCursor));
            m_copyPathLabel->installEventFilter(this);
            optionLyt->addWidget(m_copyPathLabel);
            break;
        }
        default:
            break;
    }
    optionLyt->addStretch();
    optionFrame->setLayout(optionLyt);
    m_mainLyt->addWidget(optionFrame);
}

/**
 * @brief OptionView::setupAppOptions 为应用类型的详情页构建选项表
 */
void OptionView::setupAppOptions() {
    setupOptionLabel(Options::Open);
    setupOptionLabel(Options::Shortcut);
    setupOptionLabel(Options::Panel);
}

/**
 * @brief OptionView::setupFileOptions 为文件类型的详情页构建选项表
 */
void OptionView::setupFileOptions() {
    setupOptionLabel(Options::Open);
    setupOptionLabel(Options::OpenPath);
    setupOptionLabel(Options::CopyPath);
}

/**
 * @brief OptionView::setupDirOptions 为文件夹类型的详情页构建选项表
 */
void OptionView::setupDirOptions() {
    setupOptionLabel(Options::Open);
    setupOptionLabel(Options::OpenPath);
    setupOptionLabel(Options::CopyPath);
}

/**
 * @brief OptionView::setupSettingOptions 为设置类型的详情页构建选项表
 */
void OptionView::setupSettingOptions() {
    setupOptionLabel(Options::Open);
}

/**
 * @brief OptionView::eventFilter 相应鼠标点击事件并发出信号供detailview处理
 * @param watched
 * @param event
 * @return
 */
bool OptionView::eventFilter(QObject *watched, QEvent *event){
    if (m_openLabel && watched == m_openLabel && event->type() == QEvent::MouseButtonPress){
        Q_EMIT onOptionClicked(Options::Open);
        return true;
    } else if (m_shortcutLabel && watched == m_shortcutLabel && event->type() == QEvent::MouseButtonPress) {
        Q_EMIT onOptionClicked(Options::Shortcut);
        return true;
    } else if (m_panelLabel && watched == m_panelLabel && event->type() == QEvent::MouseButtonPress) {
        Q_EMIT onOptionClicked(Options::Panel);
        return true;
    } else if (m_openPathLabel && watched == m_openPathLabel && event->type() == QEvent::MouseButtonPress) {
        Q_EMIT onOptionClicked(Options::OpenPath);
        return true;
    } else if (m_copyPathLabel && watched == m_copyPathLabel && event->type() == QEvent::MouseButtonPress) {
        Q_EMIT onOptionClicked(Options::CopyPath);
        return true;
    }
    return QObject::eventFilter(watched, event);
}
