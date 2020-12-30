INCLUDEPATH += $$PWD

include(index/index.pri)
include(appsearch/appsearch.pri)
include(settingsearch/settingsearch.pri)

HEADERS += \
    $$PWD/app-match.h \

SOURCES += \
    $$PWD/app-match.cpp \
