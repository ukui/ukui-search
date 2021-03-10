#include "friso-utils.h"

friso_t FrisoUtils::g_friso;
friso_config_t FrisoUtils::g_config;

int FrisoUtils::init()
{
    FrisoUtils::g_friso = friso_new();
    FrisoUtils::g_config = friso_new_config();
    if(friso_init_from_ifile(FrisoUtils::g_friso, FrisoUtils::g_config, FRISO_INI_PATH) != 1){
        perror("fail to initialize friso and config.\n");
        FrisoUtils::destroy();
        return -1;
    }
    return 0;
}

int FrisoUtils::destroy()
{
    friso_free_config(FrisoUtils::g_config);
    friso_free(FrisoUtils::g_friso);
    return 0;
}

//DON'T USE IN OTHER WAY!!!!!!
//MouseZhangZh
void FrisoUtils::segementOnlyInSearch(std::string& target_str, char* text)
{
    friso_task_t task = friso_new_task();

    friso_set_text( task, text); //memory!!!

    while ((FrisoUtils::g_config->next_token(FrisoUtils::g_friso, FrisoUtils::g_config, task)) != NULL){
//        printf("%s/ ", task->token->word);
        target_str += task->token->word;
        target_str += " ";
    }
    friso_free_task(task);
}

