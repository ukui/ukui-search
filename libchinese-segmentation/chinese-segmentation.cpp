#include "chinese-segmentation.h"
#include <QFileInfo>
#include <QDebug>
static ChineseSegmentation *global_instance_chinese_segmentation = nullptr;
QMutex  ChineseSegmentation::m_mutex;

ChineseSegmentation::ChineseSegmentation()
{
    const char * const DICT_PATH = "/usr/share/ukui-search/res/dict/jieba.dict.utf8";
    const char * const  HMM_PATH = "/usr/share/ukui-search/res/dict/hmm_model.utf8";
    const char * const USER_DICT_PATH ="/usr/share/ukui-search/res/dict/user.dict.utf8";
    const char * const  IDF_PATH = "/usr/share/ukui-search/res/dict/idf.utf8";
    const char * const  STOP_WORD_PATH = "/usr/share/ukui-search/res/dict/stop_words.utf8";

    m_jieba = new cppjieba::Jieba(DICT_PATH,
                                  HMM_PATH,
                                  USER_DICT_PATH,
                                  IDF_PATH,
                                  STOP_WORD_PATH);
}

ChineseSegmentation::~ChineseSegmentation()
{
    if(m_jieba)
        delete m_jieba;
    m_jieba = nullptr;
}

ChineseSegmentation *ChineseSegmentation::getInstance()
{
    QMutexLocker locker(&m_mutex);
    if (!global_instance_chinese_segmentation) {
        global_instance_chinese_segmentation = new ChineseSegmentation;
    }
    return global_instance_chinese_segmentation;
}

QVector<SKeyWord> ChineseSegmentation::callSegement(QString& str)
{
    std::string s;
    s=str.toStdString();

    const size_t topk = -1;
    std::vector<cppjieba::KeywordExtractor::Word> keywordres;
    ChineseSegmentation::m_jieba->extractor.Extract(s, keywordres, topk);
    QVector<SKeyWord> vecNeeds;
    convert(keywordres, vecNeeds);

    keywordres.clear();
    keywordres.shrink_to_fit();


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
