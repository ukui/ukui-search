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

#ifdef __cplusplus
extern "C"{
#endif

#include "friso-interface.h"

#ifdef __cplusplus
}
#endif

#include <string>
#include <vector>
#include <mutex>
#include <map>
#include <list>
#include "libfriso-segmentation_global.h"

namespace friso {
/*
using SkeyWord = ::std::pair<::std::vector<size_t>, int>;
using ResultMap = ::std::map<::std::string, ::friso::SkeyWord>;

typedef struct wordInfo{
    size_t  weight      = 0;        //the times which word appeared, need be modified.
    size_t  offsets[8]  = { 0 };    //the position which word appears.
    char    word[64]    = { 0 };    //the length of task->token->word is 64.
}fgnb;
*/
class LIBFRISOSEGMENTATION_EXPORT FrisoSegmentation
{
public:
    static FrisoSegmentation *getInstance();
    static void deleteInstance();
//    void callSegement(::friso::ResultMap&, char*);
//    void setText(char*);
    static friso_t g_friso;
    friso_config_t config;
    friso_task_t task;
private:
    explicit FrisoSegmentation();
    ~FrisoSegmentation();
//        friso::FrisoSegmentation* m_friso;
    static std::mutex m_mutex;
    int init();
//    int segment(::std::vector<::friso::SkeyWord>&, char*);
    int destory();
    char* m_ini_path = "/usr/share/ukui-search/res/friso.ini";
};
}



#endif // FRISOSEGMENTATION_H
