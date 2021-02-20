#include "friso-segmentation.h"

//static friso::FrisoSegmentation* global_instance_friso_segmentation = new (::std::nothrow)friso::FrisoSegmentation();

void friso::FrisoSegmentation::callSegement(::std::vector<friso::SkeyWord> &, std::string &)
{

}

friso::FrisoSegmentation::FrisoSegmentation()
{
    this->init();
}

int friso::FrisoSegmentation::init()
{
    this->friso = friso_new();
    this->config = friso_new_config();
    if(friso_init_from_ifile(this->friso, this->config, this->m_ini_path) != 1){
        perror("fail to initialize friso and config.\n");
        this->destory();
        return -1;
    }
    return 0;
}

int friso::FrisoSegmentation::segment()
{
    friso_task_t task = friso_new_task();
    fstring text = "这里是要被分词的字符串";
    friso_set_text( task, text );
    //friso_next 获取下一个切分结果
    //得到的切分结果存放在 task->hits 中.
    //通过 task->hits->word 的到切分的词条.
    //通过 task->hits->offset 得到对应词条在原文中的偏移位置.
//    while ( ( friso_next( friso, config, task ) ) != NULL ) {
//    //查看 friso_hits_t 可以获取更多信息。
//    //printf("%s[%d]/ ", task->hits->word, task->hits->offset );
//    printf("%s/ ", task->hits->word );
//    }
    //从 1.6.1 开始，为了方便或者，friso_hits_t 更改为 friso_token_t

    while ( ( config->next_token( friso, config, task ) ) != NULL ) {
    //查看 friso_token_t 可以获取更多信息。
    //printf("%s[%d]/ ", task->token->word, task->token->offset );
    printf("%s/ ", task->token->word );
    }

    //4. 释放资源...
    friso_free_task( task );
}

int friso::FrisoSegmentation::destory()
{
    friso_free_config(this->config);
    friso_free(this->friso);
    return 0;
}
