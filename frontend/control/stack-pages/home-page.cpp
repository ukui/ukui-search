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
#include "home-page.h"
using namespace Zeeker;

#define BACKGROUND_COLOR QColor(0, 0, 0, 0)
#define MAIN_SPACING 16
#define MAIN_MARGINS 0,0,0,0

HomePage::HomePage(QWidget *parent) : QScrollArea(parent)
{
    initUi();
    registerSections();
}

void HomePage::initUi()
{
    QPalette pal = palette();
    pal.setColor(QPalette::Base, BACKGROUND_COLOR);
    pal.setColor(QPalette::Window, BACKGROUND_COLOR); //使用此palette的窗口背景将为透明
    m_widget = new QWidget(this);
    this->setWidget(m_widget);
    m_mainLyt = new QVBoxLayout(m_widget);
    m_mainLyt->setSpacing(MAIN_SPACING);
    m_mainLyt->setContentsMargins(MAIN_MARGINS);
    m_widget->setLayout(m_mainLyt);
    m_widget->setFixedWidth(this->width());
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->setPalette(pal);
    this->setFrameShape(QFrame::Shape::NoFrame);
}

/**
 * @brief HomePage::appendSection 向homepgae添加一个版块
 * @param section
 */
void HomePage::appendSection(HomePageSection *section)
{
    section->setFixedWidth(this->width());
    m_mainLyt->addWidget(section);
    if (m_widget->height()) {
        m_widget->setFixedHeight(m_widget->height() + section->height() + MAIN_SPACING);
    } else {
        m_widget->setFixedHeight(m_widget->height() + section->height());
    }
}

//以下为homepage各版块的信息获取的回调

//NEW_TODO
//获取快速打开应用的列表
QVector<HomePageItem> get_quickly_cb()
{
    QVector<HomePageItem> quickly_list;
    QStringList quicklyOpenList;
    quicklyOpenList << "/usr/share/applications/ksc-defender.desktop"
                    << "/usr/share/applications/ukui-notebook.desktop"
                    << "/usr/share/applications/eom.desktop"
                    << "/usr/share/applications/pluma.desktop"
                    << "/usr/share/applications/claws-mail.desktop" ;
    Q_FOREACH (QString path, quicklyOpenList) {
        if (QString::compare(FileUtils::getAppName(path), "Unknown App") == 0)
            continue;
        HomePageItem item;
        item.icon = FileUtils::getAppIcon(path);
        item.name = FileUtils::getAppName(path);
        item.key = path;
        item.action = "open";
        item.pluginId = "applications";
        quickly_list.append(item);
    }
    return quickly_list;
}

//NEW_TODO 需要读写配置文件
//获取最近打开的列表
QVector<HomePageItem> get_recently_cb()
{
//    QVector<HomePageItem> recently_list;
//    return recently_list;
    return get_quickly_cb();
}

//NEW_TODO 需要读写配置文件
//获取常用应用的列表
QVector<HomePageItem> get_commonly_cb()
{
//    QVector<HomePageItem> commonly_list;
//    return commonly_list;
    return get_quickly_cb();
}

/**
 * @brief HomePage::registerSections 在此注册所有的版块
 */
void HomePage::registerSections()
{
    //快速打开
    createSection(tr("Open Quickly"), HomePageItemShape::Square, get_quickly_cb());
    //最近打开
    createSection(tr("Recently Opened"), HomePageItemShape::Bar, get_recently_cb());
    //常用应用
    createSection(tr("Commonly Used"), HomePageItemShape::Square, get_commonly_cb());
}

void HomePage::createSection(const QString &section_name, const HomePageItemShape &shape, QVector<HomePageItem> items)
{
    HomePageSection *section = new HomePageSection(section_name, shape, m_widget);
    section->setItems(items);
    if (section->length())
        this->appendSection(section);
    connect(section, &HomePageSection::requestAction, this, [ = ](const QString &key, const QString &action, const QString &pluginId) {
        SearchPluginIface *plugin = SearchPluginManager::getInstance()->getPlugin(pluginId);
//        if (plugin) {
//            plugin->openAction(action, key);
//        } else {
//            qWarning()<<"Get plugin failed!";
//        }
    });
}
