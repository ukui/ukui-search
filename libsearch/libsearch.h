/*
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
 * Authors: zhangpengfei <zhangpengfei@kylinos.cn>
 *
 */
#ifndef LIBSEARCH_H
#define LIBSEARCH_H

#include "libsearch_global.h"
#include "appsearch/app-match.h"
#include "settingsearch/setting-match.h"
#include "file-utils.h"
#include "global-settings.h"

#include "index/searchmethodmanager.h"
#include "index/first-index.h"
#include "index/ukui-search-qdbus.h"
#include "index/inotify-index.h"
#include "index/search-manager.h"

class  LIBSEARCH_EXPORT GlobalSearch {
public:

    static QStringList fileSearch(QString keyword, int begin = 0, int num = -1);

private:
    GlobalSearch();
};

#endif // LIBSEARCH_H
