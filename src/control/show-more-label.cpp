#include "show-more-label.h"
#include <QEvent>
#include <QDebug>

ShowMoreLabel::ShowMoreLabel(QWidget *parent) : QWidget(parent)
{
    initUi();
    m_timer = new QTimer;
    connect(m_timer, &QTimer::timeout, this, &ShowMoreLabel::refreshLoadState);
    connect(this, &ShowMoreLabel::showMoreClicked, this, &ShowMoreLabel::startLoading);
}

ShowMoreLabel::~ShowMoreLabel()
{
}

void ShowMoreLabel::resetLabel()
{
    m_textLabel->setText(tr("Show More..."));
}

void ShowMoreLabel::initUi()
{
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(0,0,0,6);
    m_textLabel = new QLabel(this);
    m_textLabel->setText(tr("Show More..."));
    m_textLabel->setCursor(QCursor(Qt::PointingHandCursor));
    m_textLabel->installEventFilter(this);
//    m_loadingIconLabel = new QLabel(this); //使用图片显示加载状态时，取消此label的注释
//    m_loadingIconLabel->setFixedSize(18, 18);
//    m_loadingIconLabel->hide();
    m_layout->setAlignment(Qt::AlignRight);
    m_layout->addWidget(m_textLabel);
    m_textLabel->setStyleSheet("QLabel{font-size: 14px; color: #3790FA}");
//    m_layout->addWidget(m_loadingIconLabel);
}

void ShowMoreLabel::startLoading()
{
//    m_textLabel->hide();
//    m_loadingIconLabel->show();
    m_timer->start(0.4 * 1000);
    m_textLabel->setCursor(QCursor(Qt::ArrowCursor));
}

void ShowMoreLabel::stopLoading()
{
//    m_loadingIconLabel->hide();
//    m_textLabel->show();
    if (m_timer->isActive()) {
        m_timer->stop();
    }
    if (m_isOpen) {
        m_textLabel->setText(tr("Retract"));
    } else {
        m_textLabel->setText(tr("Show More..."));
    }
    m_textLabel->setCursor(QCursor(Qt::PointingHandCursor));
}

void ShowMoreLabel::refreshLoadState()
{
    switch (m_currentState) {
        case 0: {
            m_textLabel->setText(tr("Loading"));
            m_currentState ++;
            break;
        }
        case 1: {
            m_textLabel->setText(tr("Loading."));
            m_currentState ++;
            break;
        }
        case 2: {
            m_textLabel->setText(tr("Loading.."));
            m_currentState ++;
            break;
        }
        case 3: {
            m_textLabel->setText(tr("Loading..."));
            m_currentState = 0;
            break;
        }
        default:
            break;
    }
}

bool ShowMoreLabel::eventFilter(QObject *watched, QEvent *event){
    if (watched == m_textLabel) {
        if (event->type() == QEvent::MouseButtonPress) {
            if (! m_timer->isActive()) {
                if (!m_isOpen) {
                    m_isOpen = true;
                    Q_EMIT this->showMoreClicked();
                } else {
                    m_isOpen = false;
                    Q_EMIT this->retractClicked();
                }
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}
