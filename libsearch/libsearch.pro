QT += core gui concurrent xml

VERSION = 0.0.1

TARGET = ukui-search
TEMPLATE = lib
DEFINES += LIBSEARCH_LIBRARY

PKGCONFIG += gio-2.0 glib-2.0 gio-unix-2.0
CONFIG += c++11 link_pkgconfig no_keywords



# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(index/index.pri)
include(appsearch/appsearch.pri)
include(settingsearch/settingsearch.pri)

LIBS += -lxapian -lgsettings-qt -lquazip5

SOURCES += \
    file-utils.cpp \
    globalsettings.cpp \
    gobject-template.cpp

HEADERS += \
    file-utils.h \
    globalsettings.h \
    gobject-template.h \
    libsearch_global.h \
    libsearch.h

RESOURCES += \
    resource1.qrc



# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libchinese-segmentation/release/ -lchinese-segmentation
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libchinese-segmentation/debug/ -lchinese-segmentation
else:unix: LIBS += -L$$OUT_PWD/../libchinese-segmentation/ -lchinese-segmentation

INCLUDEPATH += $$PWD/../libchinese-segmentation
DEPENDPATH += $$PWD/../libchinese-segmentation
