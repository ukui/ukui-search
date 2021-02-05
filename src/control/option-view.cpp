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
#include "option-view.h"
#include <QDebug>
#include <QEvent>

OptionView::OptionView(QWidget *parent) : QWidget(parent)
{
    m_mainLyt = new QVBoxLayout(this);
    this->setLayout(m_mainLyt);
    m_mainLyt->setContentsMargins(0,8,0,0);
    m_mainLyt->setSpacing(8);
    initUI();
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
void OptionView::setupOptions(const int& type) {
    this->hideOptions();
    switch (type) {
        case SearchListView::ResType::App : {
            setupAppOptions();
            break;
        }
        case SearchListView::ResType::Content:
        case SearchListView::ResType::Best:
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


void OptionView::initUI()
{
    m_optionFrame = new QFrame(this);
    m_optionLyt = new QVBoxLayout(m_optionFrame);
    m_optionLyt->setContentsMargins(8, 0, 0, 0);

    m_openLabel = new QLabel(m_optionFrame);
    m_openLabel->setText(tr("Open")); //打开
    m_openLabel->setStyleSheet("QLabel{font-size: 14px; color: #3790FA}");
    m_openLabel->setCursor(QCursor(Qt::PointingHandCursor));
    m_openLabel->installEventFilter(this);
    m_optionLyt->addWidget(m_openLabel);

    m_shortcutLabel = new QLabel(m_optionFrame);
    m_shortcutLabel->setText(tr("Add Shortcut to Desktop")); //添加到桌面快捷方式
    m_shortcutLabel->setStyleSheet("QLabel{font-size: 14px; color: #3790FA}");
    m_shortcutLabel->setCursor(QCursor(Qt::PointingHandCursor));
    m_shortcutLabel->installEventFilter(this);
    m_optionLyt->addWidget(m_shortcutLabel);

    m_panelLabel = new QLabel(m_optionFrame);
    m_panelLabel->setText(tr("Add Shortcut to Panel")); //添加到任务栏快捷方式
    m_panelLabel->setStyleSheet("QLabel{font-size: 14px; color: #3790FA}");
    m_panelLabel->setCursor(QCursor(Qt::PointingHandCursor));
    m_panelLabel->installEventFilter(this);
    m_optionLyt->addWidget(m_panelLabel);

    m_openPathLabel = new QLabel(m_optionFrame);
    m_openPathLabel->setText(tr("Open path")); //打开所在路径
    m_openPathLabel->setStyleSheet("QLabel{font-size: 14px; color: #3790FA}");
    m_openPathLabel->setCursor(QCursor(Qt::PointingHandCursor));
    m_openPathLabel->installEventFilter(this);
    m_optionLyt->addWidget(m_openPathLabel);

    m_copyPathLabel = new QLabel(m_optionFrame);
    m_copyPathLabel->setText(tr("Copy path")); //复制所在路径
    m_copyPathLabel->setStyleSheet("QLabel{font-size: 14px; color: #3790FA}");
    m_copyPathLabel->setCursor(QCursor(Qt::PointingHandCursor));
    m_copyPathLabel->installEventFilter(this);
    m_optionLyt->addWidget(m_copyPathLabel);

    m_optionLyt->addStretch();
    m_optionFrame->setLayout(m_optionLyt);
    m_mainLyt->addWidget(m_optionFrame);
    this->hideOptions();
}

/**
 * @brief setupOptionLabel 创建每一个单独的选项
 * @param opt 选项类型
 */
void OptionView::setupOptionLabel(const int& opt) {
    switch (opt) {
        case Options::Open: {
            m_openLabel->show();
            break;
        }
        case Options::Shortcut: {
            m_shortcutLabel->show();
            break;
        }
        case Options::Panel: {
            m_panelLabel->show();
            break;
        }
        case Options::OpenPath: {
            m_openPathLabel->show();
            break;
        }
        case Options::CopyPath: {
            m_copyPathLabel->show();
            break;
        }
        default:
            break;
    }
}

void OptionView::hideOptions()
{
    m_openLabel->hide();
    m_shortcutLabel->hide();
    m_panelLabel->hide();
    m_openPathLabel->hide();
    m_copyPathLabel->hide();
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
    if (m_openLabel && watched == m_openLabel){
        if (event->type() == QEvent::MouseButtonPress) {
            m_openLabel->setStyleSheet("QLabel{font-size: 14px; color: #296CD9;}");
            return true;
        } else if (event->type() == QEvent::MouseButtonRelease) {
            m_openLabel->setStyleSheet("QLabel{font-size: 14px; color: #3790FA}");
            Q_EMIT this->onOptionClicked(Options::Open);
            return true;
        } else if (event->type() == QEvent::Enter) {
            m_openLabel->setStyleSheet("QLabel{font-size: 14px; color: #40A9FB;}");
            return true;
        } else if (event->type() == QEvent::Leave) {
            m_openLabel->setStyleSheet("QLabel{font-size: 14px; color: #3790FA}");
            return true;
        }
    } else if (m_shortcutLabel && watched == m_shortcutLabel) {
        if (event->type() == QEvent::MouseButtonPress) {
            m_shortcutLabel->setStyleSheet("QLabel{font-size: 14px; color: #296CD9;}");
            return true;
        } else if (event->type() == QEvent::MouseButtonRelease) {
            m_shortcutLabel->setStyleSheet("QLabel{font-size: 14px; color: #3790FA}");
            Q_EMIT this->onOptionClicked(Options::Shortcut);
            return true;
        } else if (event->type() == QEvent::Enter) {
            m_shortcutLabel->setStyleSheet("QLabel{font-size: 14px; color: #40A9FB;}");
            return true;
        } else if (event->type() == QEvent::Leave) {
            m_shortcutLabel->setStyleSheet("QLabel{font-size: 14px; color: #3790FA}");
            return true;
        }
    } else if (m_panelLabel && watched == m_panelLabel) {
        if (event->type() == QEvent::MouseButtonPress) {
            m_panelLabel->setStyleSheet("QLabel{font-size: 14px; color: #296CD9;}");
            return true;
        } else if (event->type() == QEvent::MouseButtonRelease) {
            m_panelLabel->setStyleSheet("QLabel{font-size: 14px; color: #3790FA}");
            Q_EMIT this->onOptionClicked(Options::Panel);
            return true;
        } else if (event->type() == QEvent::Enter) {
            m_panelLabel->setStyleSheet("QLabel{font-size: 14px; color: #40A9FB;}");
            return true;
        } else if (event->type() == QEvent::Leave) {
            m_panelLabel->setStyleSheet("QLabel{font-size: 14px; color: #3790FA}");
            return true;
        }
    } else if (m_openPathLabel && watched == m_openPathLabel) {
        if (event->type() == QEvent::MouseButtonPress) {
            m_openPathLabel->setStyleSheet("QLabel{font-size: 14px; color: #296CD9;}");
            return true;
        } else if (event->type() == QEvent::MouseButtonRelease) {
            m_openPathLabel->setStyleSheet("QLabel{font-size: 14px; color: #3790FA}");
            Q_EMIT this->onOptionClicked(Options::OpenPath);
            return true;
        } else if (event->type() == QEvent::Enter) {
            m_openPathLabel->setStyleSheet("QLabel{font-size: 14px; color: #40A9FB;}");
            return true;
        } else if (event->type() == QEvent::Leave) {
            m_openPathLabel->setStyleSheet("QLabel{font-size: 14px; color: #3790FA}");
            return true;
        }
    } else if (m_copyPathLabel && watched == m_copyPathLabel) {
        if (event->type() == QEvent::MouseButtonPress) {
            m_copyPathLabel->setStyleSheet("QLabel{font-size: 14px; color: #296CD9;}");
            return true;
        } else if (event->type() == QEvent::MouseButtonRelease) {
            m_copyPathLabel->setStyleSheet("QLabel{font-size: 14px; color: #3790FA}");
            Q_EMIT this->onOptionClicked(Options::CopyPath);
            return true;
        } else if (event->type() == QEvent::Enter) {
            m_copyPathLabel->setStyleSheet("QLabel{font-size: 14px; color: #40A9FB;}");
            return true;
        } else if (event->type() == QEvent::Leave) {
            m_copyPathLabel->setStyleSheet("QLabel{font-size: 14px; color: #3790FA}");
            return true;
        }
    }
    return QObject::eventFilter(watched, event);
}
