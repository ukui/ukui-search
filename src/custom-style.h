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

#ifndef CUSTOMSTYLE_H
#define CUSTOMSTYLE_H
#include <QProxyStyle>
#include "global-settings.h"

class CustomStyle : public QProxyStyle {
    Q_OBJECT
public:
    static CustomStyle *getStyle();
    virtual QSize sizeFromContents(QStyle::ContentsType type, const QStyleOption *option, const QSize &contentsSize, const QWidget *widget = nullptr) const;
private:
    explicit CustomStyle(QStyle *style = nullptr);
    ~CustomStyle() override {}
};

#endif // CUSTOMSTYLE_H
