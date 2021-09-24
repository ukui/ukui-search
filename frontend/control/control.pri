include(stack-pages/stack-pages.pri)
include(list-labels/list-labels.pri)

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/search-line-edit.h \
        $$PWD/settings-widget.h \
        $$PWD/create-index-ask-dialog.h \

SOURCES += \
    $$PWD/search-line-edit.cpp \
        $$PWD/settings-widget.cpp \
        $$PWD/create-index-ask-dialog.cpp \
