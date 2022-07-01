//
// Created by hxf on 22-6-16.
//

#include "ui-config.h"

#include <QDebug>

using namespace UkuiSearch;

static UIConfig *instance = nullptr;

//default-properties
#define DEFAULT_RADIUS       6
#define DEFAULT_MARGIN       0
#define DEFAULT_SPACING      0
#define DEFAULT_COLOR        "#FFFFFF"
#define TRANSPARENT          "transparent"

//generic
#define BASE                 "base"

//keyword input
#define KEYWORD_BODY         "keywordBody"
#define KEYWORD_ICON         "keywordIcon"
#define KEYWORD_INPUT        "keywordInput"
#define KEYWORD_FONT         "keywordFont"

//new
#define RESULT_VIEW          "resultView"

//details
#define DETAILS              "details"
#define DETAILS_IMAGE        "detailsImage"
#define DETAILS_INFO         "detailsInfo"
#define DETAILS_DATA         "detailsData"
#define DETAILS_ACTION       "detailsAction"

//pluginList
#define PLUGIN_VIEW          "pluginView"
#define PLUGIN_LIST          "pluginList"
#define SCROLLER_BAR         "scrollBar"

//plugin
#define PLUGIN               "plugin"

#define PLUGIN_INFO          "pluginInfo"
#define PLUGIN_NAME          "pluginName"
#define PLUGIN_BUTTON        "pluginButton"

#define RESULT_LIST          "resultList"
#define RESULT_ITEM          "resultItem"
#define ITEM_ICON            "itemIcon"
#define ITEM_TEXT            "itemText"

//status
#define SELECT               "select"
#define HOVER                "hover"
#define LEAVE                "leave"


UIConfig *UIConfig::getInstance(QObject *parent)
{
    if (!instance) {
        instance = new UIConfig(parent);
    }

    return instance;
}

UIConfig::UIConfig(QObject *parent) : QObject(parent)
{
    loadConfig();
}

void UIConfig::loadConfig()
{
    //color
    m_colors.insert(BASE, {"#FFFFFF"});
    m_colors.insert(KEYWORD_BODY, {TRANSPARENT});

    m_colors.insert(RESULT_VIEW, {"#FFFFFF"});
    m_colors.insert(DETAILS, {"#F0F0F0"});
    m_colors.insert(DETAILS_ACTION, {"#3790FA"});

    m_colors.insert(PLUGIN_VIEW, {TRANSPARENT});
    m_colors.insert(SCROLLER_BAR, {"#E4E4E4"});

    m_colors.insert(PLUGIN, {"red"});
    m_colors.insert(PLUGIN_INFO, {"#EFEFEF"});
    m_colors.insert(RESULT_LIST, {"#FFFFFF"});
    m_colors.insert(RESULT_ITEM, {"transparent"});

    m_colors.insert(SELECT, {"#3790FA"});
    m_colors.insert(HOVER, {"#E4E4E4"});
    m_colors.insert(LEAVE, {TRANSPARENT});

    //size
    m_size.insert(KEYWORD_ICON, {24, 24});
    m_size.insert(KEYWORD_FONT, {16, 16});
    m_size.insert(PLUGIN_INFO, {0, 30});
    m_size.insert(RESULT_ITEM, {0, 36});
    m_size.insert(ITEM_ICON, {24, 24});
    m_size.insert(DETAILS_IMAGE, {128, 128});
    m_size.insert(SCROLLER_BAR, {10, 0});

    //radius
    m_radius.insert(BASE, 6);
    m_radius.insert(DETAILS, 8);
    m_radius.insert(PLUGIN_VIEW, 8);
    m_radius.insert(PLUGIN_INFO, 6);
    m_radius.insert(RESULT_LIST, 6);
    m_radius.insert(RESULT_ITEM, 6);

    //margins
    m_margins.insert(BASE, {8, 8, 8, 8});
    m_margins.insert(KEYWORD_BODY, {16, 0, 16, 0});
    m_margins.insert(RESULT_VIEW, {8, 8, 8, 8});
    m_margins.insert(DETAILS, {8, 50, 8, 0});
    m_margins.insert(PLUGIN_VIEW, {0, 0, 0, 0});
    m_margins.insert(PLUGIN_LIST, {0, 0, 0, 0});
    m_margins.insert(PLUGIN_INFO, {16, 0, 0, 0});
    m_margins.insert(RESULT_LIST, {0, 0, 5, 0});
    m_margins.insert(RESULT_ITEM, {16, 0, 0, 0});

    //spacing
    m_spacing.insert(BASE, 5);
    m_spacing.insert(KEYWORD_BODY, 4);
    m_spacing.insert(RESULT_VIEW, 5);
    m_spacing.insert(DETAILS, 8);
    m_spacing.insert(PLUGIN_VIEW, 5);
    m_spacing.insert(PLUGIN, 5);
    m_spacing.insert(PLUGIN_LIST, 5);
    m_spacing.insert(PLUGIN_INFO, 6);
    m_spacing.insert(RESULT_LIST, 0);
    m_spacing.insert(RESULT_ITEM, 8);
}

QString UIConfig::color(const QString &key)
{
    if (m_colors.contains(key)) {
        return m_colors.value(key);
    }

    return {DEFAULT_COLOR};
}

int UIConfig::height(const QString &key)
{
    if (m_size.contains(key)) {
        return m_size.value(key).height();
    }

    return 0;
}

int UIConfig::width(const QString &key)
{
    if (m_size.contains(key)) {
        return m_size.value(key).width();
    }

    return 0;
}

QSize UIConfig::size(const QString &key)
{
    if (m_size.contains(key)) {
        return m_size.value(key);
    }

    return {0, 0};
}

int UIConfig::radius(const QString &key)
{
    if (m_radius.contains(key)) {
        return m_radius.value(key);
    }

    return DEFAULT_RADIUS;
}

int UIConfig::margin(const QString &key, int pos)
{
    if (m_margins.contains(key)) {
        switch (pos) {
            case 0:
                return m_margins.value(key).left;
            case 1:
                return m_margins.value(key).top;
            case 2:
                return m_margins.value(key).right;
            case 3:
                return m_margins.value(key).bottom;
            default:
                break;
        }
    }

    return DEFAULT_MARGIN;
}

int UIConfig::spacing(const QString &key)
{
    if (m_spacing.contains(key)) {
        return m_spacing.value(key);
    }

    return DEFAULT_SPACING;
}
