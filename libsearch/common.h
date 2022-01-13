/*
 * Copyright (C) 2022, KylinSoft Co., Ltd.
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
 * Authors: zhangpengfei <zhangpengfei@kylinos.cn>
 *
 */
#ifndef COMMON_H
#define COMMON_H
#include <QMap>
#include <QString>
#include <QDir>
#define UKUI_SEARCH_PIPE_PATH (QDir::homePath()+"/.config/org.ukui/ukui-search/ukuisearch").toLocal8Bit().constData()
#define LABEL_MAX_WIDTH 320
#define HOME_PATH QDir::homePath()
static const QStringList allAppPath = {
    QDir::homePath()+"/.local/share/applications/",
    "/usr/share/applications/"
};
static const QMap<QString, bool> targetFileTypeMap = {
    std::map<QString, bool>::value_type("doc", true),
    std::map<QString, bool>::value_type("docx", true),
    std::map<QString, bool>::value_type("ppt", true),
    std::map<QString, bool>::value_type("pptx", true),
    std::map<QString, bool>::value_type("xls", true),
    std::map<QString, bool>::value_type("xlsx", true),
    std::map<QString, bool>::value_type("txt", true),
    std::map<QString, bool>::value_type("dot", true),
    std::map<QString, bool>::value_type("wps", true),
    std::map<QString, bool>::value_type("pps", true),
    std::map<QString, bool>::value_type("dps", true),
    std::map<QString, bool>::value_type("et", true),
    std::map<QString, bool>::value_type("pdf", true)
};
//TODO Put things that needed to be put here here.
#endif // COMMON_H
