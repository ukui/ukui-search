/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
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

#include "style.h"
#include <QDebug>

Style::Style()
{

}

//默认窗口
int Style::minw=0;
int Style::minh=0;
int Style::defaultMainViewWidWidth=0;
int Style::defaultTopWidHeight=0;
int Style::defaultQueryLineEditWidth=0;
int Style::defaultQueryLineEditIconSize=0;
int Style::defaultContentWidHeight=0;
int Style::defaultSideBarWidWidth=0;
/**
  * 全屏窗口
  */
//主视图
int Style::MainViewWidWidth=0;
//搜索栏
int Style::TopWidgetHeight=0;
int Style::QueryLineEditWidth=0;
int Style::QueryLineEditFontSize=0;
int Style::QueryLineEditIconSize=0;
//左侧栏
int Style::LeftWidWidth=0;
int Style::LeftWidHeight=0;
int Style::LeftMargin=0;
int Style::RightMargin=0;
int Style::LeftBtnWidth=0;
int Style::LeftBtnHeight=0;
int Style::LeftLetterBtnHeight=0;
int Style::LeftFontSize=0;
int Style::LeftLetterFontSize=0;
int Style::LeftIconSize=0;
int Style::LeftSpaceIconLeft=0;
int Style::LeftSpaceIconText=0;
int Style::LeftSpaceBetweenItem=0;
//应用列表栏
int Style::AppListWidWidth=0;
int Style::AppListWidHeight=0;
int Style::AppListFontSize=0;
int Style::AppListIconSize=0;
int Style::AppListItemSizeWidth=0;
int Style::AppListGridSizeWidth=0;
int Style::AppLeftSpace=0;
int Style::AppTopSpace=0;
int Style::AppSpaceBetweenIconText=0;
int Style::AppBottomSpace=0;
int Style::SliderSize=0;

void Style::initWidStyle()
{
    QGSettings* setting=new QGSettings(QString("org.mate.interface").toLocal8Bit());
    QString value=setting->get("font-name").toString();
    QStringList valstr=value.split(" ");
    int fontSize=valstr.at(valstr.count()-1).toInt();

    int len=0;
    QString locale = QLocale::system().name();
    if (locale == "zh_CN")
        len=0;
    else
        len=10;

    minw=QApplication::desktop()->width();//主窗口大小调整点
    minh=QApplication::desktop()->height();//主窗口大小调整点
    defaultMainViewWidWidth=678;  //MainViewWidWidth窗口大小调整点
    defaultTopWidHeight=54;
    defaultQueryLineEditWidth=288;
    defaultContentWidHeight=200;
}
