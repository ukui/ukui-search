#include "friso-segmentation.h"

static friso::FrisoSegmentation* global_instance_friso_segmentation = nullptr;
std::mutex friso::FrisoSegmentation::m_mutex;
friso_t friso::FrisoSegmentation::FrisoSegmentation::g_friso;

friso::FrisoSegmentation *friso::FrisoSegmentation::getInstance()
{
    if (global_instance_friso_segmentation == nullptr){
        ::std::unique_lock<::std::mutex> lock(m_mutex);
        if (global_instance_friso_segmentation == nullptr){
            global_instance_friso_segmentation = new (::std::nothrow) FrisoSegmentation();
            if (global_instance_friso_segmentation == nullptr){
                perror("bad alloc!\n");
                exit(-1);
            }
        }
    }
    return global_instance_friso_segmentation;
}

void friso::FrisoSegmentation::deleteInstance()
{
    ::std::unique_lock<::std::mutex> lock(m_mutex);
    if (global_instance_friso_segmentation){
        delete global_instance_friso_segmentation;
        global_instance_friso_segmentation = nullptr;
    }
}
/*
void ::friso::FrisoSegmentation::setText(char* text){
    //one friso_new_task per thread.
    //Maybe it doesn't fit thread pool.
    this->task = friso_new_task();
    friso_set_text(this->task, text);
}
*/
/*
void ::friso::FrisoSegmentation::callSegement(::friso::ResultMap &ret, char* text)
{
    ::friso::ResultMap::iterator it_map;
    friso_set_text( task, text );
    while ( ( config->next_token( FrisoSegmentation::g_friso, config, task ) ) != NULL ) {
        //查看 friso_token_t 可以获取更多信息。
        //printf("%s[%d]/ ", task->token->word, task->token->offset );
//        printf("%s/ ", task->token->word );
        it_map = ret.find(task->token->word);
        if (it_map == ret.end()){
            ::friso::SkeyWord temp_SkeyWord(::std::vector<size_t>(task->token->offset), 1);
            ret.insert(::std::pair<::std::string, ::friso::SkeyWord>(task->token->word, temp_SkeyWord));
        }
        else{
            it_map->second.first.push_back(task->token->offset);
            ++(it_map->second.second);
        }
        if(ret.size() > 1024){
            break;
        }
    }
}
*/
friso::FrisoSegmentation::FrisoSegmentation()
{
    this->init();
}

friso::FrisoSegmentation::~FrisoSegmentation()
{
    this->destory();
}

int friso::FrisoSegmentation::init()
{
    FrisoSegmentation::g_friso = friso_new();
    this->config = friso_new_config();
    if(friso_init_from_ifile(FrisoSegmentation::g_friso, this->config, this->m_ini_path) != 1){
        perror("fail to initialize friso and config.\n");
        this->destory();
        return -1;
    }
    return 0;
}
/*
int friso::FrisoSegmentation::segment(::std::vector<::friso::SkeyWord>& ret, char* text)
{
    friso_set_text( task, text );
    while ( ( config->next_token( FrisoSegmentation::g_friso, config, task ) ) != NULL ) {
        //查看 friso_token_t 可以获取更多信息。
        //printf("%s[%d]/ ", task->token->word, task->token->offset );
        printf("%s/ ", task->token->word );
    }
}
*/
int friso::FrisoSegmentation::destory()
{
    //4. 释放资源...
//    friso_free_task(this->task);
    friso_free_config(this->config);
    friso_free(FrisoSegmentation::g_friso);
    return 0;
}
