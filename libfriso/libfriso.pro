QT -= gui

VERSION = 0.0.1
TARGET =  friso
TEMPLATE = lib
DEFINES += LIBFRISO_LIBRARY

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
include(friso/friso.pri)

SOURCES += \
    friso-interface.c

HEADERS += \
    friso-interface.h \

dict_utf_files.path = /usr/share/ukui-search/res/dict/UTF-8/
dict_utf_files.files = $$PWD/friso/vendors/dict/UTF-8/*
dict_gbk_files.path = /usr/share/ukui-search/res/dict/GBK/
dict_gbk_files.files = $$PWD/friso/vendors/dict/GBK/*

friso_ini.path = /usr/share/ukui-search/res/
friso_ini.files = $$PWD/friso/friso.ini

INSTALLS += \
    dict_utf_files \
    dict_gbk_files \
    friso_ini

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    friso/vendors/dict/UTF-8/* \
    friso/vendors/dict/GBK/* \
    friso/friso.ini


