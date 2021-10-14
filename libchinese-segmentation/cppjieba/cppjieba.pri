INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/DictTrie.hpp \
    $$PWD/IdfTrie.hpp \
    $$PWD/FullSegment.hpp \
    $$PWD/HMMModel.hpp \
    $$PWD/HMMSegment.hpp \
    $$PWD/Jieba.hpp \
    $$PWD/KeywordExtractor.hpp \
    $$PWD/MPSegment.hpp \
    $$PWD/MixSegment.hpp \
    $$PWD/PosTagger.hpp \
    $$PWD/PreFilter.hpp \
    $$PWD/QuerySegment.hpp \
    $$PWD/SegmentBase.hpp \
    $$PWD/SegmentTagged.hpp \
    $$PWD/TextRankExtractor.hpp \
    $$PWD/Unicode.hpp
include(limonp/limonp.pri)
