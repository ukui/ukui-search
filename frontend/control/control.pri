include(stack-pages/stack-pages.pri)
include(flow-layout/flow-layout.pri)
include(list-labels/list-labels.pri)

INCLUDEPATH += $$PWD

HEADERS += \
        $$PWD/settings-widget.h \
        $$PWD/create-index-ask-dialog.h \
        $$PWD/stacked-widget.h \
        $$PWD/input-box.h \

SOURCES += \
        $$PWD/settings-widget.cpp \
        $$PWD/create-index-ask-dialog.cpp \
        $$PWD/stacked-widget.cpp \
        $$PWD/input-box.cpp \
