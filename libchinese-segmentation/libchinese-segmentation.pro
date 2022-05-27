QT -= gui

VERSION = 0.0.1
TARGET =  chinese-segmentation
TEMPLATE = lib
DEFINES += LIBCHINESESEGMENTATION_LIBRARY

CONFIG += c++11 create_pc create_prl no_install_prl

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
QMAKE_CXXFLAGS += -Werror=return-type -Werror=return-local-addr
#QMAKE_CXXFLAGS += -Werror=uninitialized
QMAKE_CXXFLAGS += -execution-charset:utf-8

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
include(cppjieba/cppjieba.pri)

#LIBS += -L/usr/local/lib/libjemalloc -ljemalloc

SOURCES += \
    chinese-segmentation.cpp \
    hanzi-to-pinyin.cpp

HEADERS += \
    chinese-segmentation-private.h \
    chinese-segmentation.h \
    common-struct.h \
    hanzi-to-pinyin-private.h \
    hanzi-to-pinyin.h \
    libchinese-segmentation_global.h

dict_files.path = /usr/share/ukui-search/res/dict/
dict_files.files = $$PWD/dict/*.utf8\
dict_files.files += $$PWD/dict/pos_dict/*.utf8\
dict_files.files += $$PWD/dict/*.txt\

INSTALLS += \
    dict_files \

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_LIBS]
    QMAKE_PKGCONFIG_NAME = chinese-segmentation
    QMAKE_PKGCONFIG_DESCRIPTION = chinese-segmentation Header files
    QMAKE_PKGCONFIG_VERSION = $$VERSION
    QMAKE_PKGCONFIG_LIBDIR = $$target.path
    QMAKE_PKGCONFIG_DESTDIR = pkgconfig
    QMAKE_PKGCONFIG_INCDIR = /usr/include/chinese-seg
    QMAKE_PKGCONFIG_CFLAGS += -I/usr/include/chinese-seg

!isEmpty(target.path): INSTALLS += target

    header.path = /usr/include/chinese-seg
    header.files += chinese-segmentation.h libchinese-segmentation_global.h common-struct.h hanzi-to-pinyin.h
    header.files += development-files/header-files/*
#    headercppjieba.path = /usr/include/chinese-seg/cppjieba/
#    headercppjieba.files = cppjieba/*
    INSTALLS += header
}


#DISTFILES += \
#    jiaba/jieba.pri

DISTFILES += \
    dict/README.md \
    dict/hmm_model.utf8 \
    dict/idf.utf8 \
    dict/jieba.dict.utf8 \
    dict/pos_dict/char_state_tab.utf8 \
    dict/pos_dict/prob_emit.utf8 \
    dict/pos_dict/prob_start.utf8 \
    dict/pos_dict/prob_trans.utf8 \
    dict/stop_words.utf8 \
    dict/user.dict.utf8 \
    dict/pinyinWithoutTone.txt \
    development-files/header-files/* \
