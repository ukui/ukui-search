TEMPLATE = subdirs
SUBDIRS += $$PWD/libchinese-segmentation \
           $$PWD/libsearch \
           $$PWD/frontend \
           $$PWD/ukuisearch-systemdbus \
           $$PWD/search-ukcc-plugin \
           $$PWD/ukui-search-service \
           $$PWD/ukui-search-app-data-service \
           $$PWD/ukui-search-service-dir-manager
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

libsearch.depends += libchinese-segmentation
ukui-search-service.depends += libsearch
#src.depends = libsearch
frontend.depends = libsearch

CONFIG += ordered \

QT += widgets


