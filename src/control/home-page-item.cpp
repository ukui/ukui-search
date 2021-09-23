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
#include "home-page-item.h"
#include <QEvent>
#include <QProcess>
#include <QDebug>
#include <gio/gdesktopappinfo.h>
#include <QPainter>
#include <QDesktopServices>
#include <QUrl>
#include <config-file.h>

using namespace Zeeker;
HomePageItem::HomePageItem(QWidget *parent, const int& type, const QString& path) : QWidget(parent) {
    setupUi(type, path);
    m_transparency = 0.06;
    connect(qApp, &QApplication::paletteChanged, this, [ = ]() {
        if(m_namelabel) {
            QString name = this->toolTip();
            if(m_type == ItemType::Recent) {
                m_namelabel->setText(m_namelabel->fontMetrics().elidedText(name, Qt::ElideRight, 250));
            } else {
                m_namelabel->setText(m_namelabel->fontMetrics().elidedText(name, Qt::ElideRight, 108));
            }
        }
    });
}

HomePageItem::~HomePageItem() {
}

/**
 * @brief HomePageItem::setupUi 根据不同的分栏创建item
 * @param type 所属分栏
 * @param path 路径
 */
void HomePageItem::setupUi(const int& type, const QString& path) {
    m_path = path;
    m_type = type;
    m_widget = new QWidget(this);
    m_widget->setObjectName("MainWidget");
    m_widget->installEventFilter(this);
    m_iconlabel = new QLabel(m_widget);
    m_namelabel = new QLabel(m_widget);
    if(type == ItemType::Recent) {
        m_widget->setFixedSize(300, 48);
        QIcon icon;
        switch(SearchListView::getResType(path)) {  //可能出现文件应用等，需要根据路径判断类型
        case SearchListView::ResType::App : {
            icon = FileUtils::getAppIcon(path);
            m_namelabel->setText(FileUtils::getAppName(path));
            QFontMetrics fontMetrics = m_namelabel->fontMetrics();
            QString name = FileUtils::getAppName(path);
            m_namelabel->setText(fontMetrics.elidedText(name, Qt::ElideRight, 250));
            this->setToolTip(name);
            break;
        }
        case SearchListView::ResType::Best :
        case SearchListView::ResType::Content:
        case SearchListView::ResType::Dir :
        case SearchListView::ResType::File : {
            icon = FileUtils::getFileIcon(QUrl::fromLocalFile(path).toString());
//                m_namelabel->setText(FileUtils::getFileName(path));
            QFontMetrics fontMetrics = m_namelabel->fontMetrics();
            QString name = FileUtils::getFileName(path);
            m_namelabel->setText(fontMetrics.elidedText(name, Qt::ElideRight, 250));
            this->setToolTip(name);
            break;
        }
        case SearchListView::ResType::Setting : {
            icon = FileUtils::getSettingIcon(path, true);
//                m_namelabel->setText(FileUtils::getSettingName(path));
            QFontMetrics fontMetrics = m_namelabel->fontMetrics();
            QString name = FileUtils::getSettingName(path);
            m_namelabel->setText(fontMetrics.elidedText(name, Qt::ElideRight, 250));
            this->setToolTip(name);
            break;
        }
        default :
            break;
        }
        m_iconlabel->setPixmap(icon.pixmap(icon.actualSize(QSize(24, 24))));
        m_hlayout = new QHBoxLayout(m_widget);
        m_iconlabel->setAlignment(Qt::AlignCenter);
        m_namelabel->setAlignment(Qt::AlignCenter);
        m_hlayout->addWidget(m_iconlabel);
        m_hlayout->addWidget(m_namelabel);
        m_hlayout->addStretch();
        return;
    } else if(type == ItemType::Quick) {
        QIcon icon = FileUtils::getAppIcon(path);
        m_iconlabel->setPixmap(icon.pixmap(icon.actualSize(QSize(48, 48))));
        QString name = FileUtils::getAppName(path);
        m_namelabel->setText(m_namelabel->fontMetrics().elidedText(name, Qt::ElideRight, 108));
        this->setToolTip(name);
    } else {
        QIcon icon = FileUtils::getAppIcon(path);
        m_iconlabel->setPixmap(icon.pixmap(icon.actualSize(QSize(48, 48))));
//        m_namelabel->setText(FileUtils::getAppName(path));
        QFontMetrics fontMetrics = m_namelabel->fontMetrics();
        QString name = FileUtils::getAppName(path);
        m_namelabel->setText(fontMetrics.elidedText(name, Qt::ElideRight, 108));
        this->setToolTip(name);
    }
    m_widget->setFixedSize(116, 116);
    m_vlayout = new QVBoxLayout(m_widget);
    m_vlayout->setContentsMargins(0, 16, 0, 12);
    m_iconlabel->setAlignment(Qt::AlignCenter);
    m_namelabel->setAlignment(Qt::AlignCenter);
    m_vlayout->addWidget(m_iconlabel);
    m_vlayout->addWidget(m_namelabel);
}

void HomePageItem::onItemClicked() {
    switch(SearchListView::getResType(m_path)) {
    case SearchListView::ResType::App: {
        GDesktopAppInfo * desktopAppInfo = g_desktop_app_info_new_from_filename(m_path.toLocal8Bit().data());
        g_app_info_launch(G_APP_INFO(desktopAppInfo), nullptr, nullptr, nullptr);
        g_object_unref(desktopAppInfo);
        break;
    }
    case SearchListView::ResType::Best:
    case SearchListView::ResType::Content:
    case SearchListView::ResType::Dir:
    case SearchListView::ResType::File: {
        QDesktopServices::openUrl(QUrl::fromLocalFile(m_path));
        break;
    }
    case SearchListView::ResType::Setting: {
        //打开控制面板对应页面
        QProcess process;
        process.startDetached(QString("ukui-control-center --%1").arg(m_path.left(m_path.indexOf("/")).toLower()));
        break;
    }
    default:
        break;
    }
    ConfigFile::getInstance()->writeConfig(m_path);
}

bool HomePageItem::eventFilter(QObject *watched, QEvent *event) {
    if(watched == m_widget) {
        if(event->type() == QEvent::MouseButtonPress) {
            m_transparency = 0.06;
            this->update();
            return true;
        } else if(event->type() == QEvent::MouseButtonRelease) {
            this->onItemClicked();
            m_transparency = 0.06;
            this->update();
            return true;
        } else if(event->type() == QEvent::Enter) {
            m_transparency = 0.15;
            this->update();
            return true;
        } else if(event->type() == QEvent::Leave) {
            m_transparency = 0.06;
            this->update();
            return true;
        }
    }

    return QObject::eventFilter(watched, event);
}

void HomePageItem::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QRect rect = this->rect();
    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    p.setBrush(opt.palette.color(QPalette::Text));
    p.setOpacity(m_transparency);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rect, 4, 4);
    return QWidget::paintEvent(event);
}
