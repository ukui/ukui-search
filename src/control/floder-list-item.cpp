#include "floder-list-item.h"
#include <QIcon>
#include <QEvent>

FloderListItem::FloderListItem(QWidget *parent, const QString &path) : QWidget(parent)
{
    m_path = path;
    initUi();
}

FloderListItem::~FloderListItem()
{

}

/**
 * @brief FloderListItem::initUi 构建ui
 */
void FloderListItem::initUi() {
    m_layout = new QVBoxLayout(this);
    m_layout->setSpacing(0);
    m_layout->setContentsMargins(0,0,0,0);
    m_widget = new QWidget(this);
    m_widget->setObjectName("mWidget");
    this->setFixedHeight(32);
    m_layout->addWidget(m_widget);
    m_widgetlayout = new QHBoxLayout(m_widget);
    m_widgetlayout->setContentsMargins(4, 4, 4, 4);
    m_widget->setLayout(m_widgetlayout);

    m_iconLabel = new QLabel(m_widget);
    m_pathLabel = new QLabel(m_widget);
    m_delLabel = new QLabel(m_widget);
    m_iconLabel->setPixmap(QIcon::fromTheme("inode-directory").pixmap(QSize(16, 16)));
    m_pathLabel->setText(m_path);
    m_delLabel->setText(tr("Delete the floder out of blacklist"));
    m_pathLabel->setStyleSheet("QLabel{color: palette(text); background: transparent;}");
    m_delLabel->setStyleSheet("QLabel{color: #3790FA; background: transparent;}");
    m_delLabel->setCursor(QCursor(Qt::PointingHandCursor));
    m_delLabel->installEventFilter(this);
    m_delLabel->hide();
    m_widgetlayout->addWidget(m_iconLabel);
    m_widgetlayout->addWidget(m_pathLabel);
    m_widgetlayout->addStretch();
    m_widgetlayout->addWidget(m_delLabel);
}

/**
 * @brief FloderListItem::enterEvent 鼠标移入事件
 * @param event
 */
void FloderListItem::enterEvent(QEvent *event){
    m_delLabel->show();
    m_widget->setStyleSheet("QWidget#mWidget{background: rgba(0,0,0,0.1);}");
    QWidget::enterEvent(event);
}

/**
 * @brief FloderListItem::leaveEvent 鼠标移出事件
 * @param event
 */
void FloderListItem::leaveEvent(QEvent *event){
    m_delLabel->hide();
    m_widget->setStyleSheet("QWidget#mWidget{background: transparent;}");
    QWidget::leaveEvent(event);
}


/**
 * @brief FloderListItem::eventFilter 处理删除按钮点击事件
 * @param watched
 * @param event
 * @return
 */
bool FloderListItem::eventFilter(QObject *watched, QEvent *event){
    if (watched == m_delLabel) {
        if (event->type() == QEvent::MouseButtonPress) {
//            qDebug()<<"pressed!";
        }
    }
    return QObject::eventFilter(watched, event);
}
