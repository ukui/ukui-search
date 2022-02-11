QT += core gui dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = ukui-search-service
VERSION = 1.0.0
DEFINES += VERSION='\\"$${VERSION}\\"'
CONFIG += c++11 link_pkgconfig no_keywords lrelease
PKGCONFIG += gsettings-qt


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

SOURCES += \
        main.cpp

# Default rules for deployment.
target.path = /usr/bin
INSTALLS += target
