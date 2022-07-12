QT += core gui dbus sql KWindowSystem

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = ukui-search-app-data-service
VERSION = 1.0.0
DEFINES += VERSION='\\"$${VERSION}\\"'
CONFIG += c++11 no_keywords

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
QMAKE_CXXFLAGS += -Werror=return-type -Werror=return-local-addr -Werror=uninitialized

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#include(../libsearch/appdata/appdata.pri)
include(../3rd-parties/qtsingleapplication/qtsingleapplication.pri)

LIBS += -lQt5Xdg

SOURCES += \
        main.cpp \
        convert-winid-to-desktop.cpp \
        app-db-manager.cpp \
    pending-app-info-queue.cpp \
        signal-transformer.cpp \
        ukui-search-app-data-service.cpp \

HEADERS += \
        convert-winid-to-desktop.h \
        app-db-manager.h \
    pending-app-info-queue.h \
    pending-app-info.h \
        signal-transformer.h \
        ukui-search-app-data-service.h \

target.path = /usr/bin
INSTALLS += target

desktop.path = /etc/xdg/autostart
desktop.files += ../data/ukui-search-app-data-service.desktop
INSTALLS += desktop

LIBS += -L$$OUT_PWD/../libchinese-segmentation/ -lchinese-segmentation
INCLUDEPATH += $$PWD/../libchinese-segmentation
DEPENDPATH += $$PWD/../libchinese-segmentation

LIBS += -L$$OUT_PWD/../libsearch/ -lukui-search
INCLUDEPATH += $$PWD/../libsearch
DEPENDPATH += $$PWD/../libsearch
