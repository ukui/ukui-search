QT += dbus widgets

TEMPLATE = lib
DEFINES += SEARCHAPPWIDGETPLUGIN_LIBRARY

CONFIG += c++11 link_pkgconfig

PKGCONFIG += gsettings-qt

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TRANSLATIONS += translations/zh_CN.ts

SOURCES += \
    search.cpp

HEADERS += \
    search-app-widget-plugin_global.h \
    search.h

LIBS += -lukui-appwidget-manager -lukui-appwidget-provider

# Default rules for deployment.

qml.files += provider/data/search.qml
qml.path = /usr/share/appwidget/qml/

qm_files.files = translations/* $$OUT_PWD/.qm/*.qm
qm_files.path = /usr/share/appwidget/search/translations/

appwidgetconf.files  += provider/data/search.conf
appwidgetconf.path = /usr/share/appwidget/config/

service.files += provider/org.ukui.appwidget.provider.search.service
service.path += /usr/share/dbus-1/services/

preview.files += provider/data/search.png
preview.path = /usr/share/appwidget/search/

svg.files += provider/data/ukui-search.svg
svg.path = /usr/share/appwidget/search/

INSTALLS += target qml qm_files appwidgetconf service preview svg
