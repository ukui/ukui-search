QT += widgets

TEMPLATE = lib
TARGET = $$qtLibraryTarget(search-ukcc-plugin)
target.path = $$[QT_INSTALL_LIBS]/ukui-control-center/

CONFIG += plugin link_pkgconfig c++11 lrelease

PKGCONFIG += gio-2.0 gio-unix-2.0 gsettings-qt kysdk-qtwidgets

LIBS +=  -L$$[QT_INSTALL_LIBS] -lgsettings-qt
LIBS += -lukcc

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += search.cpp

HEADERS += search.h

TRANSLATIONS += translations/zh_CN.ts \
                translations/bo_CN.ts

images.files = image/*
images.path = /usr/share/ukui-search/search-ukcc-plugin/image/

qm_files.files = translations/* $$OUT_PWD/.qm/*.qm
qm_files.path = /usr/share/ukui-search/search-ukcc-plugin/translations


INSTALLS += target images qm_files

DISTFILES += \
    ./image/360.svg \
    ./image/baidu.scg \
    ./image/sougou.svg \
    ./image/add.svg
