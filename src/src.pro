QT       += core gui dbus  KWindowSystem xml x11extras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

VERSION = 0.0.1
TARGET = ukui-search
TEMPLATE = app

PKGCONFIG += gio-2.0 glib-2.0 gio-unix-2.0
CONFIG += c++11 link_pkgconfig no_keywords lrelease
LIBS += -lxapian -lgsettings-qt -lquazip5 -lX11
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(model/model.pri)
include(control/control.pri)
include(singleapplication/qt-single-application.pri)

SOURCES += \
    content-widget.cpp \
    input-box.cpp \
    main.cpp \
    mainwindow.cpp \
    search-result.cpp \
    settings-widget.cpp \
    xatom-helper.cpp


HEADERS += \
    content-widget.h \
    input-box.h \
    mainwindow.h \
    search-result.h \
    settings-widget.h \
    xatom-helper.h

# Default rules for deployment.

target.path = /usr/bin
!isEmpty(target.path): INSTALLS += target

data-menu.path = /usr/share/applications
data-menu.files += ../data/ukui-search-menu.desktop
data.path = /etc/xdg/autostart
data.files += ../data/ukui-search.desktop
INSTALLS += data data-menu

RESOURCES += \
    resource.qrc

TRANSLATIONS += \
    ../translations/ukui-search/zh_CN.ts \
    ../translations/ukui-search/tr.ts \
    ../translations/ukui-search/bo.ts

qm_files.path = /usr/share/ukui-search/translations/
qm_files.files = $$OUT_PWD/.qm/*.qm

INSTALLS += qm_files

LIBS += -L$$OUT_PWD/../libsearch -lukui-search -L$$OUT_PWD/../libchinese-segmentation/ -lchinese-segmentation

INCLUDEPATH += $$PWD/../libsearch
DEPENDPATH += $$PWD/../libsearch

DISTFILES += \
    ../data/ukui-search-menu.desktop \
    translations/bo.ts \
    translations/es.qm \
    translations/zh_CN.qm
