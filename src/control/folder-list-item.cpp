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
#include "folder-list-item.h"
#include <QIcon>
#include <QEvent>

FolderListItem::FolderListItem(QWidget *parent, const QString &path) : QWidget(parent) {
    m_path = path;
    initUi();
}

FolderListItem::~FolderListItem() {

}

/**
 * @brief FolderListItem::initUi 构建ui
 */
void FolderListItem::initUi() {
    m_layout = new QVBoxLayout(this);
    m_layout->setSpacing(0);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_widget = new QWidget(this);
    m_widget->setObjectName("mWidget");
    this->setFixedHeight(32);
    m_layout->addWidget(m_widget);
    m_widgetlayout = new QHBoxLayout(m_widget);
    m_widgetlayout->setContentsMargins(8, 4, 8, 4);
    m_widget->setLayout(m_widgetlayout);

    m_iconLabel = new QLabel(m_widget);
    m_pathLabel = new QLabel(m_widget);
    m_delLabel = new QLabel(m_widget);
    m_iconLabel->setPixmap(QIcon::fromTheme("inode-directory").pixmap(QSize(16, 16)));
    m_pathLabel->setText(m_path);
    m_delLabel->setText(tr("Delete the folder out of blacklist"));
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
 * @brief FolderListItem::getPath 获取当前文件夹路径
 * @return
 */
QString FolderListItem::getPath() {
    return m_path;
}

/**
 * @brief FolderListItem::enterEvent 鼠标移入事件
 * @param event
 */
void FolderListItem::enterEvent(QEvent *event) {
    m_delLabel->show();
    m_widget->setStyleSheet("QWidget#mWidget{background: rgba(0,0,0,0.1);}");
    QWidget::enterEvent(event);
}

/**
 * @brief FolderListItem::leaveEvent 鼠标移出事件
 * @param event
 */
void FolderListItem::leaveEvent(QEvent *event) {
    m_delLabel->hide();
    m_widget->setStyleSheet("QWidget#mWidget{background: transparent;}");
    QWidget::leaveEvent(event);
}


/**
 * @brief FolderListItem::eventFilter 处理删除按钮点击事件
 * @param watched
 * @param event
 * @return
 */
bool FolderListItem::eventFilter(QObject *watched, QEvent *event) {
    if(watched == m_delLabel) {
        if(event->type() == QEvent::MouseButtonPress) {
//            qDebug()<<"pressed!";
            Q_EMIT this->onDelBtnClicked(m_path);
        }
    }
    return QObject::eventFilter(watched, event);
}
