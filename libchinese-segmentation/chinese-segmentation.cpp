#include "chinese-segmentation.h"
#include <QFileInfo>

ChineseSegmentation::ChineseSegmentation()
{
}

QVector<SKeyWord> ChineseSegmentation::callSegement(QString *str)
{
    const char * const DICT_PATH = "/usr/share/ukui-search/res/dict/jieba.dict.utf8";
    const char * const  HMM_PATH = "/usr/share/ukui-search/res/dict/hmm_model.utf8";
    const char * const USER_DICT_PATH ="/usr/share/ukui-search/res/dict/user.dict.utf8";
    const char * const  IDF_PATH = "/usr/share/ukui-search/res/dict/idf.utf8";
    const char * const  STOP_WORD_PATH = "/usr/share/ukui-search/res/dict/stop_words.utf8";


    cppjieba::Jieba jieba(DICT_PATH,
           HMM_PATH,
           USER_DICT_PATH,
           IDF_PATH,
           STOP_WORD_PATH);

    std::string s;
    s=str->toStdString();

    const size_t topk = -1;
    std::vector<cppjieba::KeywordExtractor::Word> keywordres;
    jieba.extractor.Extract(s, keywordres, topk);
    QVector<SKeyWord> vecNeeds;
    convert(keywordres, vecNeeds);

    return vecNeeds;

}

void ChineseSegmentation::convert(std::vector<cppjieba::KeywordExtractor::Word> &keywordres, QVector<SKeyWord> &kw)
{
    for (auto i : keywordres){
        SKeyWord temp;
        temp.word = i.word;
        temp.offsets = QVector<size_t>::fromStdVector(i.offsets);
        temp.weight = i.weight;
        kw.append(temp);
    }
}
