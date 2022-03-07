#ifndef COMMON_H
#define COMMON_H
#include <QMap>
#include <QString>
#include <QDir>
#define UKUI_SEARCH_PIPE_PATH (QDir::homePath()+"/.config/org.ukui/ukui-search/ukuisearch").toLocal8Bit().constData()
#define FILE_SEARCH_VALUE "0"
#define DIR_SEARCH_VALUE "1"
#define LABEL_MAX_WIDTH 300
#define HOME_PATH QDir::homePath()
#define INDEX_SEM "ukui-search-index-sem"
static const QStringList allAppPath = {
    QDir::homePath()+"/.local/share/applications/",
    "/usr/share/applications/"
};
static const QMap<QString, bool> targetFileTypeMap = {
    std::map<QString, bool>::value_type("doc", true),
    std::map<QString, bool>::value_type("docx", true),
    std::map<QString, bool>::value_type("ppt", true),
    std::map<QString, bool>::value_type("pptx", true),
    std::map<QString, bool>::value_type("xls", true),
    std::map<QString, bool>::value_type("xlsx", true),
    std::map<QString, bool>::value_type("txt", true),
    std::map<QString, bool>::value_type("dot", true),
    std::map<QString, bool>::value_type("wps", true),
    std::map<QString, bool>::value_type("pps", true),
    std::map<QString, bool>::value_type("dps", true),
    std::map<QString, bool>::value_type("et", true),
    std::map<QString, bool>::value_type("pdf", true)
};

static const QMap<QString, bool> targetPhotographTypeMap = {
    std::map<QString, bool>::value_type("png", true),
    std::map<QString, bool>::value_type("jpg", true),
    std::map<QString, bool>::value_type("jpeg", true)//TODO 待完善,后续改为配置文件
};
//TODO Put things that needed to be put here here.
#endif // COMMON_H
