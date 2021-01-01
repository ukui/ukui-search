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

struct SKeyWord{
    std::string word;
    QVector<size_t> offsets;
    double weight;
};

class CHINESESEGMENTATION_EXPORT ChineseSegmentation
{
public:
    static QVector<SKeyWord> callSegement(QString *str);
    static void convert(std::vector<cppjieba::KeywordExtractor::Word>& keywordres,QVector<SKeyWord>& kw);
private:
    ChineseSegmentation();
};

#endif // CHINESESEGMENTATION_H
