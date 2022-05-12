#pragma once

#include <QMap>
#include <QString>
#include <QDir>

static const int LABEL_MAX_WIDTH = 300;

static const QString HOME_PATH = QDir::homePath();
static const QString INDEX_PATH = HOME_PATH + QStringLiteral("/.config/org.ukui/ukui-search/index_data");
static const QString CONTENT_INDEX_PATH = HOME_PATH + QStringLiteral("/.config/org.ukui/ukui-search/content_index_data");
static const QString OCR_INDEX_PATH = HOME_PATH + QStringLiteral("/.config/org.ukui/ukui-search/ocr_index_data");
static const QString FILE_SEARCH_VALUE = QStringLiteral("0");
static const QString DIR_SEARCH_VALUE = QStringLiteral("1");
static const QString INDEX_SEM = QStringLiteral("ukui-search-index-sem");
static const QByteArray UKUI_SEARCH_SCHEMAS = QByteArrayLiteral("org.ukui.search.settings");
static const QString SEARCH_METHOD_KEY = QStringLiteral("fileIndexEnable");

static const QStringList allAppPath = {
    {HOME_PATH + "/.local/share/applications/"},
    {"/usr/share/applications/"}
};

static const QMap<QString, bool> targetFileTypeMap = {
    {"doc", true},
    {"docx", true},
    {"ppt", true},
    {"pptx", true},
    {"xls", true},
    {"xlsx", true},
    {"txt", true},
    {"dot", true},
    {"wps", true},
    {"pps", true},
    {"dps", true},
    {"et", true},
    {"pdf", true},
    {"html", true},
    {"uof", true},
    {"uot", true},
    {"uos", true},
    {"uop", true},
    {"ofd", true}
};

static const QMap<QString, bool> targetPhotographTypeMap = {
    {"png", true},
    {"jpg", true},
    {"jpeg", true} // TODO 待完善,后续改为配置文件
};
//TODO Put things that needed to be put here here.

