/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */

#include "ukuicontrolstyle.h"
#include <QStyleOption>
#include <QPainter>
#include <QToolTip>
#include <QPalette>
#include "src/Style/style.h"

UKUiMenu::UKUiMenu(){
}

UKUiMenu::~UKUiMenu(){
}

void UKUiMenu::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.setBrush(QBrush(Qt::red));
    p.setPen(Qt::black);
    p.drawRoundedRect(opt.rect,6,6);
//    p.drawText(rect(), opt. Qt::AlignCenter,);
    style()->drawPrimitive(QStyle::PE_PanelMenu, &opt, &p, this);
}

UkuiToolButton::UkuiToolButton(){}
UkuiToolButton::~UkuiToolButton(){}
void UkuiToolButton::paintTooltipStyle()
{
    //设置QToolTip颜色
    QPalette palette = QToolTip::palette();
    palette.setColor(QPalette::Inactive,QPalette::ToolTipBase,Qt::black);   //设置ToolTip背景色
    palette.setColor(QPalette::Inactive,QPalette::ToolTipText, Qt::white); 	//设置ToolTip字体色
    QToolTip::setPalette(palette);
//    QFont font("Segoe UI", -1, 50);
//    font.setPixelSize(12);
//    QToolTip::setFont(font);  //设置ToolTip字体
}

UKUiFrame::UKUiFrame(){
}

UKUiFrame::~UKUiFrame(){
}

void UKUiFrame::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.setBrush(QBrush(QColor(0x13,0x14,0x14,0xb2)));
    p.setPen(Qt::NoPen);
    p.setRenderHint(QPainter::Antialiasing);
    p.drawRoundedRect(opt.rect,6,6);
    style()->drawPrimitive(QStyle::PE_Frame, &opt, &p, this);
}

UKUiWidget::UKUiWidget(){
}

UKUiWidget::~UKUiWidget(){
}

void UKUiWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.setBrush(QBrush(QColor(0x13,0x14,0x14,0xb2)));
    p.setPen(Qt::NoPen);
    p.setRenderHint(QPainter::Antialiasing);
    p.drawRoundedRect(opt.rect,6,6);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

UKuiMainWindow::UKuiMainWindow(QWidget *parent):
    QMainWindow(parent)
{
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setAutoFillBackground(false);
    this->setFocusPolicy(Qt::StrongFocus);
    this->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
}
UKuiMainWindow::~UKuiMainWindow()
{}

void UKuiMainWindow::paintEvent(QPaintEvent *event)
{
    QGSettings* gsetting=new QGSettings(QString("org.ukui.control-center.personalise").toLocal8Bit());
    double transparency=gsetting->get("transparency").toDouble();
    qreal radius = 0;
    QRect rect = this->rect();
    rect.setWidth(rect.width());
    rect.setHeight(rect.height());
    rect.setX(this->rect().x());
    rect.setY(this->rect().y());
    rect.setWidth(this->rect().width());
    rect.setHeight(this->rect().height());
    radius=12;

    QPainterPath path;
    path.moveTo(rect.topRight() - QPointF(radius, 0));
    path.lineTo(rect.topLeft() + QPointF(radius, 0));
    path.quadTo(rect.topLeft(), rect.topLeft() + QPointF(0, radius));
    path.lineTo(rect.bottomLeft() + QPointF(0, -radius));
    path.quadTo(rect.bottomLeft(), rect.bottomLeft() + QPointF(radius, 0));
    path.lineTo(rect.bottomRight() - QPointF(radius, 0));
    path.quadTo(rect.bottomRight(), rect.bottomRight() + QPointF(0, -radius));
    path.lineTo(rect.topRight() + QPointF(0, radius));
    path.quadTo(rect.topRight(), rect.topRight() + QPointF(-radius, -0));

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    painter.setBrush(this->palette().base());
    painter.setPen(Qt::transparent);
    painter.setOpacity(transparency);

    painter.drawPath(path);
    setProperty("blurRegion", QRegion(path.toFillPolygon().toPolygon()));
    QMainWindow::paintEvent(event);
}
