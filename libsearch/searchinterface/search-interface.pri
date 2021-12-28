INCLUDEPATH += $$PWD
include(searchtasks/search-tasks.pri)

HEADERS += \
    $$PWD/result-item-private.h \
    $$PWD/search-controller-private.h \
    $$PWD/search-controller.h \
    $$PWD/result-item.h \
    $$PWD/ukui-search-task-private.h \
    $$PWD/ukui-search-task.h

SOURCES += \
    $$PWD/search-controller.cpp \
    $$PWD/result-item.cpp \
    $$PWD/ukui-search-task.cpp

