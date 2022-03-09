QT += core gui dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = ukui-search-service
VERSION = 1.0.0
DEFINES += VERSION='\\"$${VERSION}\\"'
CONFIG += c++11 link_pkgconfig no_keywords lrelease
PKGCONFIG += gsettings-qt gio-unix-2.0

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(../libsearch/libukui-search-headers.pri)
include(../3rd-parties/qtsingleapplication/qtsingleapplication.pri)

LIBS += -L$$OUT_PWD/../libsearch -lukui-search

SOURCES += \
        main.cpp \
        ukui-search-service.cpp

schemes.path =  /usr/share/glib-2.0/schemas/
schemes.files += ../data/org.ukui.search.data.gschema.xml

INSTALLS += schemes

target.path = /usr/bin
INSTALLS += target

desktop.path = /etc/xdg/autostart
desktop.files += ../data/ukui-search-service.desktop
INSTALLS += desktop

HEADERS += \
    ukui-search-service.h
