QT += quick KWindowSystem x11extras widgets

VERSION = 1.0.0
DEFINES += VERSION='\\"$${VERSION}\\"'
TARGET = ukui-search
TEMPLATE = app

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
PKGCONFIG += gio-2.0 glib-2.0 gio-unix-2.0
CONFIG += c++11 link_pkgconfig no_keywords lrelease
LIBS += -lxapian -lgsettings-qt -lquazip5 -lX11
LIBS += -L$$OUT_PWD/../libchinese-segmentation -lchinese-segmentation \
        -L$$OUT_PWD/../libsearch -lukui-search

include(../libsearch/libukui-search-headers.pri)
include(../xatom/xatom.pri)
include(model/model.pri)


HEADERS += \
    main-view.h \
    search-line-edit-view.h \
    search-result-page-view.h

SOURCES += \
        main-view.cpp \
        main.cpp \
        search-line-edit-view.cpp \
        search-result-page-view.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
target.path = /usr/bin
!isEmpty(target.path): INSTALLS += target
