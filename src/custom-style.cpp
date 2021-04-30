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

#include "custom-style.h"

using namespace Zeeker;
static CustomStyle *customstyle_global_instance = nullptr;
CustomStyle::CustomStyle(QStyle *style) {

}
CustomStyle *CustomStyle::getStyle()
{
    if (!customstyle_global_instance)
        customstyle_global_instance = new CustomStyle;
    return customstyle_global_instance;
}

QSize CustomStyle::sizeFromContents(QStyle::ContentsType type, const QStyleOption *option, const QSize &contentsSize, const QWidget *widget) const {
    switch(type) {
    case CT_ItemViewItem: {
        QSize size(0, GlobalSettings::getInstance()->getValue(FONT_SIZE_KEY).toDouble() * 2);
        return size;
    }
    break;
    default:
        break;
    }
    return QProxyStyle::sizeFromContents(type, option, contentsSize, widget);
}


