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
#include "search-page-section.h"
#include <QDebug>
using namespace Zeeker;

#define RESULT_LAYOUT_MARGINS 0,0,0,0
#define RESULT_BACKGROUND_COLOR QColor(0, 0, 0, 0)
#define DETAIL_BACKGROUND_COLOR QColor(0, 0, 0, 0)
#define DETAIL_WIDGET_TRANSPARENT 0.04
#define DETAIL_WIDGET_BORDER_RADIUS 4
#define DETAIL_WIDGET_MARGINS 8,40,40,8
#define DETAIL_FRAME_MARGINS 8,0,8,0
#define DETAIL_ICON_HEIGHT 120
#define NAME_LABEL_WIDTH 280
#define ICON_SIZE QSize(96, 96)
#define LINE_STYLE "QFrame{background: rgba(0,0,0,0.2);}"

ResultArea::ResultArea(QWidget *parent) : QScrollArea(parent)
{
    qRegisterMetaType<SearchPluginIface::ResultInfo>("SearchPluginIface::ResultInfo");
    initUi();
}

void ResultArea::appendWidet(ResultWidget *widget)
{
    //NEW_TODO
    m_mainLyt->addWidget(widget);
    setupConnectionsForWidget(widget);
    m_widget->setFixedHeight(m_widget->height() + widget->height());
}

/**
 * @brief ResultArea::setVisibleList 设置哪些插件可见，默认全部可见
 * @param list
 */
void ResultArea::setVisibleList(const QStringList &list)
{
    Q_FOREACH (auto widget, m_widget_list) {
        if (list.contains(widget->pluginId())) {
            widget->setEnabled(true);
        } else {
            widget->setEnabled(false);
        }
    }
}

void ResultArea::initUi()
{
    QPalette pal = palette();
    pal.setColor(QPalette::Base, RESULT_BACKGROUND_COLOR);
    pal.setColor(QPalette::Window, RESULT_BACKGROUND_COLOR);
    this->setFrameShape(QFrame::Shape::NoFrame);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->setPalette(pal);
    this->setWidgetResizable(true);
    this->setFrameShape(QFrame::Shape::NoFrame);
    m_widget = new QWidget(this);
    this->setWidget(m_widget);
    m_mainLyt = new QVBoxLayout(m_widget);
    m_widget->setLayout(m_mainLyt);
    m_mainLyt->setContentsMargins(RESULT_LAYOUT_MARGINS);
}

void ResultArea::setupConnectionsForWidget(ResultWidget *widget)
{
    connect(this, &ResultArea::startSearch, widget, &ResultWidget::startSearch);
    connect(this, &ResultArea::stopSearch, widget, &ResultWidget::stopSearch);
}

DetailArea::DetailArea(QWidget *parent) : QScrollArea(parent)
{
    initUi();
    connect(this, &DetailArea::setWidgetInfo, m_detailWidget, &DetailWidget::setWidgetInfo);
}

void DetailArea::initUi()
{
    QPalette pal = palette();
    pal.setColor(QPalette::Base, DETAIL_BACKGROUND_COLOR);
    pal.setColor(QPalette::Window, DETAIL_BACKGROUND_COLOR);
    this->setPalette(pal);
    this->setFrameShape(QFrame::Shape::NoFrame);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->setWidgetResizable(true);
    m_detailWidget = new DetailWidget(this);
    this->setWidget(m_detailWidget);
}

DetailWidget::DetailWidget(QWidget *parent) : QWidget(parent)
{
    initUi();
}

QString escapeHtml(const QString & str) {
    QString temp = str;
    temp.replace("<", "&lt;");
    temp.replace(">", "&gt;");
    return temp;
}

void DetailWidget::setWidgetInfo(const QString &plugin_name, const SearchPluginIface::ResultInfo &info)
{
    m_iconLabel->setPixmap(info.icon.pixmap(info.icon.actualSize(ICON_SIZE)));
    m_iconLabel->show();
    QFontMetrics fontMetrics = m_nameLabel->fontMetrics();
    QString name = fontMetrics.elidedText(info.name, Qt::ElideRight, NAME_LABEL_WIDTH - 8);
    m_nameLabel->setText(QString("<h3 style=\"font-weight:normal;\">%1</h3>").arg(escapeHtml(name)));
    m_nameLabel->setToolTip(info.name);
    m_pluginLabel->setText(plugin_name);
    m_nameFrame->show();
    m_line_1->show();
    if (info.description.length() > 0) {
        //NEW_TODO
        clearLayout(m_descFrameLyt);
        Q_FOREACH (SearchPluginIface::DescriptionInfo desc, info.description) {
            QLabel * descLabel = new QLabel(m_descFrame);
            QString show_desc = desc.key + ":    " + desc.value;
            descLabel->setText(show_desc);
            m_descFrameLyt->addWidget(descLabel);
        }
        m_line_2->show();
    }
    clearLayout(m_actionFrameLyt);
    Q_FOREACH (auto action, info.actionMap) {
        //NEW_TODO
        QLabel * actionLabel = new QLabel(m_actionFrame);
        actionLabel->setText(action);
        m_actionFrameLyt->addWidget(actionLabel);
    }
    m_actionFrame->show();
}

void DetailWidget::clear()
{
    m_iconLabel->hide();
    m_nameFrame->hide();
    m_line_1->hide();
    m_descFrame->hide();
    m_line_2->hide();
    m_actionFrame->hide();
}

void DetailWidget::initUi()
{
    m_mainLyt = new QVBoxLayout(this);
    this->setLayout(m_mainLyt);
    m_mainLyt->setContentsMargins(DETAIL_WIDGET_MARGINS);
    m_mainLyt->setAlignment(Qt::AlignHCenter);

    m_iconLabel = new QLabel(this);
    m_iconLabel->setFixedHeight(DETAIL_ICON_HEIGHT);
    m_iconLabel->setAlignment(Qt::AlignCenter);

    m_nameFrame = new QFrame(this);
    m_nameFrameLyt = new QHBoxLayout(m_nameFrame);
    m_nameFrame->setLayout(m_nameFrameLyt);
    m_nameFrameLyt->setContentsMargins(DETAIL_FRAME_MARGINS);
    m_nameLabel = new QLabel(m_nameFrame);
    m_nameLabel->setMaximumWidth(NAME_LABEL_WIDTH);
    m_pluginLabel = new QLabel(m_nameFrame);
    m_nameFrameLyt->addWidget(m_nameLabel);
    m_nameFrameLyt->addStretch();
    m_nameFrameLyt->addWidget(m_pluginLabel);

    m_line_1 = new QFrame(this);
    m_line_1->setFixedHeight(1);
    m_line_1->setLineWidth(0);
    m_line_1->setStyleSheet(LINE_STYLE);
    m_line_2 = new QFrame(this);
    m_line_2->setFixedHeight(1);
    m_line_2->setLineWidth(0);
    m_line_2->setStyleSheet(LINE_STYLE);

    m_descFrame = new QFrame(this);
    m_descFrameLyt = new QVBoxLayout(m_descFrame);
    m_descFrame->setLayout(m_descFrameLyt);
    m_descFrameLyt->setContentsMargins(DETAIL_FRAME_MARGINS);

    m_actionFrame = new QFrame(this);
    m_actionFrameLyt = new QVBoxLayout(m_actionFrame);
    m_actionFrame->setLayout(m_actionFrameLyt);
    m_actionFrameLyt->setContentsMargins(DETAIL_FRAME_MARGINS);

    m_mainLyt->addWidget(m_iconLabel);
    m_mainLyt->addWidget(m_nameFrame);
    m_mainLyt->addWidget(m_line_1);
    m_mainLyt->addWidget(m_descFrame);
    m_mainLyt->addWidget(m_line_2);
    m_mainLyt->addWidget(m_actionFrame);
    m_mainLyt->addStretch();
}

void DetailWidget::paintEvent(QPaintEvent * event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QRect rect = this->rect();
    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    p.setBrush(opt.palette.color(QPalette::Text));
    p.setOpacity(DETAIL_WIDGET_TRANSPARENT);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rect, DETAIL_WIDGET_BORDER_RADIUS, DETAIL_WIDGET_BORDER_RADIUS);
    return QWidget::paintEvent(event);
}

void DetailWidget::clearLayout(QLayout *layout)
{
    if(! layout) return;
    QLayoutItem * child;
    while((child = layout->takeAt(0)) != 0) {
        if(child->widget()) {
            child->widget()->setParent(NULL);
        }
        delete child;
    }
    child = NULL;
}
