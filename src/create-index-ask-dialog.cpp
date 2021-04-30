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

#include "create-index-ask-dialog.h"
#include <QPainter>

using namespace Zeeker;
CreateIndexAskDialog::CreateIndexAskDialog(QWidget *parent) : QDialog(parent) {
    this->setWindowIcon(QIcon::fromTheme("kylin-search"));
    this->setWindowTitle(tr("ukui-search"));

    initUi();
}

void CreateIndexAskDialog::initUi() {
    this->setFixedSize(380, 202);
    m_mainLyt = new QVBoxLayout(this);
    this->setLayout(m_mainLyt);
    m_mainLyt->setContentsMargins(0, 0, 0, 0);
    m_mainLyt->setSpacing(0);

    //标题栏
    m_titleFrame = new QFrame(this);
    m_titleFrame->setFixedHeight(40);
    m_titleLyt = new QHBoxLayout(m_titleFrame);
    m_titleLyt->setContentsMargins(8, 8, 8, 8);
    m_titleFrame->setLayout(m_titleLyt);
    m_iconLabel = new QLabel(m_titleFrame);
    m_iconLabel->setFixedSize(24, 24);
    m_iconLabel->setPixmap(QIcon::fromTheme("kylin-search").pixmap(QSize(24, 24)));
    m_titleLabel = new QLabel(m_titleFrame);
    m_titleLabel->setText(tr("Search"));
    m_closeBtn = new QPushButton(m_titleFrame);
    m_closeBtn->setFixedSize(24, 24);
    m_closeBtn->setIcon(QIcon::fromTheme("window-close-symbolic"));
    m_closeBtn->setProperty("isWindowButton", 0x02);
    m_closeBtn->setProperty("useIconHighlightEffect", 0x08);
    m_closeBtn->setFlat(true);
    connect(m_closeBtn, &QPushButton::clicked, this, [ = ]() {
        this->hide();
        Q_EMIT this->closed();
    });
    m_titleLyt->addWidget(m_iconLabel);
    m_titleLyt->addWidget(m_titleLabel);
    m_titleLyt->addStretch();
    m_titleLyt->addWidget(m_closeBtn);

    m_mainLyt->addWidget(m_titleFrame);

    //内容区域
    m_contentFrame = new QFrame(this);
    m_contentLyt = new QVBoxLayout(m_contentFrame);
    m_contentFrame->setLayout(m_contentLyt);
    m_contentLyt->setContentsMargins(32, 0, 32, 24);

    m_tipLabel = new QLabel(m_contentFrame);
    m_tipLabel->setText(tr("Creating index can help you getting results quickly, whether to create or not?"));
    m_tipLabel->setWordWrap(true);
    m_tipLabel->setAlignment(Qt::AlignVCenter);
    m_tipLabel->setMinimumHeight(64);
    m_contentLyt->addWidget(m_tipLabel);

    m_checkFrame = new QFrame(m_contentFrame);
    m_checkLyt = new QHBoxLayout(m_checkFrame);
    m_checkLyt->setContentsMargins(0, 0, 0, 0);
    m_checkFrame->setLayout(m_checkLyt);
    m_checkBox = new QCheckBox(m_checkFrame);
    m_checkBox->setText(tr("Don't remind"));
    m_checkLyt->addWidget(m_checkBox);
    m_checkLyt->addStretch();
    m_contentLyt->addWidget(m_checkFrame);
    m_contentLyt->addStretch();

    m_btnFrame = new QFrame(m_contentFrame);
    m_btnLyt = new QHBoxLayout(m_btnFrame);
    m_btnFrame->setLayout(m_btnLyt);
    m_btnLyt->setContentsMargins(0, 0, 0, 0);
    m_cancelBtn = new QPushButton(m_btnFrame);
    m_cancelBtn->setText(tr("No"));
    m_confirmBtn = new QPushButton(m_btnFrame);
    m_confirmBtn->setText(tr("Yes"));
    connect(m_cancelBtn, &QPushButton::clicked, this, [ = ]() {
        Q_EMIT this->btnClicked(false, m_checkBox->isChecked());
        this->hide();
        Q_EMIT this->closed();
    });
    connect(m_confirmBtn, &QPushButton::clicked, this, [ = ]() {
        Q_EMIT this->btnClicked(true, m_checkBox->isChecked());
        this->hide();
        Q_EMIT this->closed();
    });
    m_btnLyt->addStretch();
    m_btnLyt->addWidget(m_cancelBtn);
    m_btnLyt->addWidget(m_confirmBtn);
    m_contentLyt->addWidget(m_btnFrame);

    m_mainLyt->addWidget(m_contentFrame);
}

/**
 * @brief CreateIndexAskDialog::paintEvent 绘制窗口背景（默认背景较暗）
 */
void CreateIndexAskDialog::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    QPainterPath rectPath;
    rectPath.addRect(this->rect());
    p.save();
    p.fillPath(rectPath, palette().color(QPalette::Base));
    p.restore();
    return QDialog::paintEvent(event);
}
