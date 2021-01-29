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
