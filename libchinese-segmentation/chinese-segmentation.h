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
#ifndef CHINESESEGMENTATION_H
#define CHINESESEGMENTATION_H

#include "libchinese-segmentation_global.h"
#include "cppjieba/Jieba.hpp"
//#include "Logging.hpp"
//#include "LocalVector.hpp"
//#include "cppjieba/QuerySegment.hpp"
#include "cppjieba/KeywordExtractor.hpp"
#include <QVector>
#include <QString>
#include <QDebug>
#include <QMutex>

struct SKeyWord {
    std::string word;
    QVector<size_t> offsets;
    double weight;
    ~SKeyWord() {
        word = std::move("");
        offsets.clear();
//        offsets.shrink_to_fit();
    }
};

class CHINESESEGMENTATION_EXPORT ChineseSegmentation {
public:
    static ChineseSegmentation *getInstance();
    ~ChineseSegmentation();
    QVector<SKeyWord> callSegement(std::string s);
    //新添加callSegementStd函数，修改返回值为std：：vector<cppjieba::KeywordExtractor::Word>并简化内部处理流程--jxx20210517
    //修改函数入参形式为引用，去掉Qstring与std::string转换代码--jxx20210519
    std::vector<cppjieba::KeywordExtractor::Word> callSegementStd(const std::string& str);
    void convert(std::vector<cppjieba::KeywordExtractor::Word>& keywordres, QVector<SKeyWord>& kw);
private:
    static QMutex m_mutex;
    cppjieba::Jieba *m_jieba;
    explicit ChineseSegmentation();

};

#endif // CHINESESEGMENTATION_H
