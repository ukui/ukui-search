QT += core xml widgets dbus concurrent sql
VERSION = 1.0.0
DEFINES += VERSION='\\"$${VERSION}\\"'

TARGET = ukui-search
TEMPLATE = lib
DEFINES += LIBSEARCH_LIBRARY
CONFIG += create_pc create_prl no_install_prl

PKGCONFIG += gio-2.0 glib-2.0 gio-unix-2.0 gsettings-qt poppler-qt5

CONFIG += c++11 link_pkgconfig no_keywords lrelease
QMAKE_CXXFLAGS += -Werror=return-type -Werror=return-local-addr -Werror=uninitialized


# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

PLUGIN_INSTALL_DIRS = $$[QT_INSTALL_LIBS]/ukui-search-plugins
DEFINES += PLUGIN_INSTALL_DIRS='\\"$${PLUGIN_INSTALL_DIRS}\\"'
QMAKE_CXXFLAGS += -execution-charset:utf-8

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
include(pluginmanage/plugin-manager.pri)
include(plugininterface/plugin-interface.pri)
include(index/index.pri)
include(parser/parser.pri)
include(appsearch/appsearch.pri)
include(appdata/appdata.pri)
include(notesearch/notesearch.pri)
include(settingsearch/settingsearch.pri)
include(websearch/websearch.pri)
include(searchinterface/search-interface.pri)
include(dirwatcher/dirwatcher.pri)
include(mailsearch/mailsearch.pri)

LIBS += -L$$OUT_PWD/../libchinese-segmentation/ -lchinese-segmentation
LIBS += -lxapian -lquazip5 -luchardet -lQt5Xdg#-L/usr/local/lib/libjemalloc -ljemalloc

SOURCES += \
    file-utils.cpp \
    global-settings.cpp \
    gobject-template.cpp \
    libsearch.cpp

HEADERS += \
    common.h \
    file-utils.h \
    global-settings.h \
    gobject-template.h \
    libsearch_global.h \
    libsearch.h

RESOURCES += \
    resource1.qrc \

TRANSLATIONS += \
    ../translations/libukui-search/libukui-search_zh_CN.ts

qm_files.path = /usr/share/ukui-search/translations/
qm_files.files = $$OUT_PWD/.qm/*.qm

INSTALLS += qm_files


# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_LIBS]
    QMAKE_PKGCONFIG_NAME = ukui-search
    QMAKE_PKGCONFIG_DESCRIPTION = Ukui-search Header files
    QMAKE_PKGCONFIG_VERSION = $$VERSION
    QMAKE_PKGCONFIG_LIBDIR = $$target.path
    QMAKE_PKGCONFIG_DESTDIR = pkgconfig
    QMAKE_PKGCONFIG_INCDIR = /usr/include/ukui-search
    QMAKE_PKGCONFIG_CFLAGS += -I/usr/include/ukui-search
    QMAKE_PKGCONFIG_REQUIRES = Qt5Widgets glib-2.0 gio-2.0 gio-unix-2.0 poppler-qt5 gsettings-qt

    INSTALLS += target

    header.path = /usr/include/ukui-search
    header.files += *.h index/*.h appsearch/*.h settingsearch/*.h plugininterface/*.h websearch/*.h \
                     searchinterface/ukui-search-task.h \
                     searchinterface/search-controller.h \
                     searchinterface/result-item.h
    header.files += development-files/header-files/*

    INSTALLS += header
}
INCLUDEPATH += $$PWD/../libchinese-segmentation
DEPENDPATH += $$PWD/../libchinese-segmentation

#DISTFILES += \
#    ../translations/libsearch/libukui-search_zh_CN.ts
