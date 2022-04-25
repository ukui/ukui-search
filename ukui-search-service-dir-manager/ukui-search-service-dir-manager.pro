QT       += core dbus
QT       -= gui

TARGET = ukui-search-service-dir-manager
VERSION = 1.0.0
DEFINES += VERSION='\\"$${VERSION}\\"'
TEMPLATE = app

CONFIG += c++11 link_pkgconfig
#CONFIG -= app_bundle
PKGCONFIG += gio-2.0 gio-unix-2.0

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(dirwatcher/dir-watcher-dbus.pri)
include(../3rd-parties/qtsingleapplication/qtsingleapplication.pri)

inst1.files += conf/com.ukui.search.fileindex.service
inst1.path = /usr/share/dbus-1/services/

target.source += $$TARGET
target.path = /usr/bin
INSTALLS += \
    target \
    inst1

desktop.path = /etc/xdg/autostart
desktop.files += ../data/ukui-search-service-dir-manager.desktop
INSTALLS += desktop

SOURCES += \
        main.cpp \
        ukui-search-dir-manager-dbus.cpp

HEADERS += \
    ukui-search-dir-manager-dbus.h
