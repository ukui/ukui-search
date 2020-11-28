#-------------------------------------------------
#
# ukui-menu
#
#-------------------------------------------------

QT       += core gui svg dbus x11extras KWindowSystem xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -g

TARGET = ukui-search
TEMPLATE = app

target.path = /usr/bin

#TRANSLATIONS+=\
#    translations/ukui-menu_bo.ts \
#    translations/ukui-menu_zh_CN.ts \
#    translations/ukui-menu_tr.ts

#QM_FILES_INSTALL_PATH = /usr/share/ukui-menu/translations/

# CONFIG += lrelase not work for qt5.6, add those from lrelease.prf for compatibility
qtPrepareTool(QMAKE_LRELEASE, lrelease)
lrelease.name = lrelease
lrelease.input = TRANSLATIONS
lrelease.output = ${QMAKE_FILE_IN_BASE}.qm
lrelease.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
lrelease.CONFIG = no_link
QMAKE_EXTRA_COMPILERS += lrelease
PRE_TARGETDEPS += compiler_lrelease_make_all

for (translation, TRANSLATIONS) {
    translation = $$basename(translation)
    QM_FILES += $$OUT_PWD/$$replace(translation, \\..*$, .qm)
}
qm_files.files = $$QM_FILES
qm_files.path = $$QM_FILES_INSTALL_PATH
qm_files.CONFIG = no_check_exist
INSTALLS += qm_files

# So we can access it from main.cpp
DEFINES += QM_FILES_INSTALL_PATH='\\"$${QM_FILES_INSTALL_PATH}\\"'

include(src/QtSingleApplication/qtsingleapplication.pri)             #单例
include(src/MainWindow/mainwindow.pri)                               #主界面
include(src/AppSearch/appsearch.pri)                                 #应用搜索
include(src/FileSearch/filesearch.pri)                               #文件搜索
include(src/ControlCenterSettingsSearch/controlcentersearch.pri)     #控制面板搜索
include(src/WebSearch/websearch.pri)                                 #网页搜索
include(src/Interface/interface.pri)                                 #接口

SOURCES += \
    src/Style/style.cpp \
    src/UtilityFunction/itemdelegate.cpp \
    src/UtilityFunction/listview.cpp \


HEADERS  += \
    src/Style/style.h \
    src/UtilityFunction/itemdelegate.h \
    src/UtilityFunction/listview.h \

RESOURCES += \
    res.qrc

CONFIG += no_keywords link_pkgconfig
PKGCONFIG+=glib-2.0 gio-unix-2.0 gsettings-qt libbamf3 x11 xrandr xtst

desktop_file.files = ukui-search.desktop
desktop_file.path = /etc/xdg/autostart

INSTALLS += \
    target desktop_file
