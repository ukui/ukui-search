QT -= gui

VERSION = 0.0.1
TARGET =  chinese-segmentation
TEMPLATE = lib
DEFINES += LIBCHINESESEGMENTATION_LIBRARY

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
include(cppjieba/cppjieba.pri)

SOURCES += \
    chinese-segmentation.cpp \

HEADERS += \
    chinese-segmentation.h \
    libchinese-segmentation_global.h

dict_files.path = /usr/share/ukui-search/res/dict/
dict_files.files = $$PWD/dict/*.utf8\
dict_files.files += $$PWD/dict/pos_dict/*.utf8\

INSTALLS += \
    dict_files \

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_LIBS]
}
!isEmpty(target.path): INSTALLS += target

    header.path = /usr/include/chinese-seg/
    header.files += *.h cppjieba/*
    INSTALLS += header header_cppjieba

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
    dict/user.dict.utf8

