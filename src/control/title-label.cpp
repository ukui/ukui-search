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
#include "title-label.h"
#include <QPainter>
#include <QStyleOption>

TitleLabel::TitleLabel(QWidget * parent) : QLabel(parent)
{
    this->setContentsMargins(8, 0, 0, 0);
    this->setFixedHeight(24);
}

TitleLabel::~TitleLabel()
{

}

void TitleLabel::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event)

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QRect rect = this->rect();
    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    p.setBrush(opt.palette.color(QPalette::Text));
    p.setOpacity(0.06);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rect, 0, 0);
    return QLabel::paintEvent(event);
}
