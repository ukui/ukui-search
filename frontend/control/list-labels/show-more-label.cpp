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
#include "show-more-label.h"
#include <QEvent>
#include <QDebug>
#include <QIcon>
using namespace Zeeker;
ShowMoreLabel::ShowMoreLabel(QWidget *parent) : QWidget(parent) {
    initUi();
    this->setAttribute(Qt::WA_AcceptTouchEvents);
    m_timer = new QTimer;
}

void ShowMoreLabel::resetLabel() {
    m_isOpen = false;
    m_textLabel->setPixmap(QIcon::fromTheme("pan-down-symbolic").pixmap(QSize(16, 16)));
}

/**
 * @brief ShowMoreLabel::getExpanded 获取当前是否是展开状态
 * @return true已展开，false已收起
 */
bool ShowMoreLabel::getExpanded() {
    return m_isOpen;
}

void ShowMoreLabel::initUi() {
    QPalette pal = palette();
    pal.setColor(QPalette::WindowText, QColor(55, 144, 250, 255));
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 6);
    m_textLabel = new QLabel(this);
    m_textLabel->setPixmap(QIcon::fromTheme("pan-down-symbolic").pixmap(QSize(16, 16)));
    m_textLabel->setCursor(QCursor(Qt::PointingHandCursor));
    m_textLabel->installEventFilter(this);
//    m_loadingIconLabel = new QLabel(this); //使用图片显示加载状态时，取消此label的注释
//    m_loadingIconLabel->setFixedSize(18, 18);
//    m_loadingIconLabel->hide();
    m_layout->setAlignment(Qt::AlignRight | Qt::AlignTop);
    m_layout->addWidget(m_textLabel);
    m_textLabel->setPalette(pal);
    m_textLabel->setCursor(QCursor(Qt::PointingHandCursor));
//    m_layout->addWidget(m_loadingIconLabel);
}

bool ShowMoreLabel::event(QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress || event->type() == QEvent::TouchBegin) {
        if(! m_timer->isActive()) {
            if(!m_isOpen) {
                m_textLabel->setPixmap(QIcon::fromTheme("pan-up-symbolic").pixmap(QSize(16, 16)));
                m_isOpen = true;
                Q_EMIT this->showMoreClicked();
            } else {
                m_textLabel->setPixmap(QIcon::fromTheme("pan-down-symbolic").pixmap(QSize(16, 16)));
                m_isOpen = false;
                Q_EMIT this->retractClicked();
            }
        }
        return false;
    }
    return QWidget::event(event);
}
