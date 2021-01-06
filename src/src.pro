QT       += core gui dbus  KWindowSystem xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

VERSION = 0.0.1
TARGET = ukui-search
TEMPLATE = app

PKGCONFIG += gio-2.0 glib-2.0 gio-unix-2.0
CONFIG += c++11 link_pkgconfig no_keywords
LIBS += -lxapian -lgsettings-qt -lquazip5
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
    settings-widget.cpp \


HEADERS += \
    content-widget.h \
    input-box.h \
    mainwindow.h \
    settings-widget.h

# Default rules for deployment.

target.path = /usr/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc

TRANSLATIONS += \
    res/translations/zh_CN.ts \
    res/translations/tr.ts \
    res/translations/bo.ts

qm_files.path = /usr/share/ukui-search/res/translations/
qm_files.files = $$OUT_PWD/res/translations/*.qm

INSTALLS += \
    qm_files \

LIBS += -L$$OUT_PWD/../libsearch -L$$OUT_PWD/../libchinese-segmentation/ -lchinese-segmentation -lukui-search

INCLUDEPATH += $$PWD/../libsearch
DEPENDPATH += $$PWD/../libsearch
