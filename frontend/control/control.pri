include(stack-pages/stack-pages.pri)
include(flow-layout/flow-layout.pri)
include(list-labels/list-labels.pri)

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/search-line-edit.h \
        $$PWD/settings-widget.h \
        $$PWD/create-index-ask-dialog.h \
        $$PWD/stacked-widget.h \

SOURCES += \
    $$PWD/search-line-edit.cpp \
        $$PWD/settings-widget.cpp \
        $$PWD/create-index-ask-dialog.cpp \
        $$PWD/stacked-widget.cpp \
