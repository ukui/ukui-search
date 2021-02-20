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
 * Authors: zhangzihao <zhangzihao@kylinos.cn>
 * Modified by: zhangpengfei <zhangpengfei@kylinos.cn>
 *
 */
#ifndef FRISOSEGMENTATION_H
#define FRISOSEGMENTATION_H

extern "C"{
#include "friso-interface.h"
}

#include <string>
#include <vector>
#include <mutex>
#include "libfriso-segmentation_global.h"
namespace friso {
    struct SkeyWord{
        std::string word;
        std::vector<size_t> offset;
        double weight;
    };

    class LIBFRISOSEGMENTATION_EXPORT FrisoSegmentation
    {
    public:
        static FrisoSegmentation *getInstance();
        ~FrisoSegmentation();
        void callSegement(::std::vector<::friso::SkeyWord>&, ::std::string&);
    private:
        explicit FrisoSegmentation();
//        friso::FrisoSegmentation* m_friso;
        static std::mutex m_mutex;
        friso_t friso;
        friso_config_t config;
        friso_task_t task;
        int init();
        int segment();
        int destory();
        char* m_ini_path = "/usr/share/ukui-search/res/friso.ini";
    };
}



#endif // FRISOSEGMENTATION_H
