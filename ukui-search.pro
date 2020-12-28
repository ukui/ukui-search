QT       += core gui svg dbus x11extras KWindowSystem xml concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += link_pkgconfig c++11 no_keywords

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(src/src.pri)
include(index/index.pri)
include(model/model.pri)
include(control/control.pri)
include(appsearch/appsearch.pri)
include(singleapplication/qt-single-application.pri)
include(settingsmatch/setting-match.pri)
include(appmatch/app-match.pri)

LIBS = -lxapian -lgsettings-qt
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    file-utils.h \
    globalsettings.h \
    gobject-template.h \

SOURCES += \
    file-utils.cpp \
    globalsettings.cpp \
    gobject-template.cpp \

PKGCONFIG += gio-2.0 glib-2.0 gio-unix-2.0 gsettings-qt libbamf3 x11 xrandr xtst

RESOURCES += \
    resource.qrc

TRANSLATIONS += \
    res/translations/zh_CN.ts \
    res/translations/tr.ts \
    res/translations/bo.ts

qm_files.path = /usr/share/ukui-search/res/translations/
qm_files.files = res/translations/*.qm\

INSTALLS += \
    qm_files \
