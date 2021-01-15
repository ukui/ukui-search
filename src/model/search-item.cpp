#include "search-item.h"
#include <QDebug>
#include <QFileInfo>

SearchItem::SearchItem(QObject *parent) : QObject(parent)
{
}

SearchItem::~SearchItem()
{
}

/**
 * @brief SearchItem::getIcon 获取一个搜索项的图标
 * @param index 条目索引
 * @return 图标
 */
QIcon SearchItem::getIcon(int index) {
    if (index < 0 || index >= m_pathlist.count())
        return QIcon("");
    switch (m_searchtype) {
        case Settings : //设置项，返回控制面板对应插件的图标
            return FileUtils::getSettingIcon(m_pathlist.at(index), false);
        case Contents:
        case Dirs :
        case Files : //文件，返回文件图标
            return FileUtils::getFileIcon(QString("file://%1").arg(m_pathlist.at(index)));
        case Apps : //应用，返回应用图标
            return FileUtils::getAppIcon(m_pathlist.at(index));
        case Best : {//最佳匹配，含全部类型，需要自己判断，返回不同类型的图标
//            return QIcon(":/res/icons/edit-find-symbolic.svg");
            if (m_pathlist.at(index).endsWith(".desktop")) {
                return FileUtils::getAppIcon(m_pathlist.at(index));
            } else if (QFileInfo(m_pathlist.at(index)).isFile() || QFileInfo(m_pathlist.at(index)).isDir()) {
                return FileUtils::getFileIcon(QString("file://%1").arg(m_pathlist.at(index)));
            } else {
                return FileUtils::getSettingIcon(m_pathlist.at(index), false);
            }
        }
        default:
            return QIcon(":/res/icons/edit-find-symbolic.svg");
    }
}

/**
 * @brief SearchItem::getIcon 获取一个搜索项的名称
 * @param index 条目索引
 * @return 名字
 */
QString SearchItem::getName(int index) {
    if (index < 0 || index >= m_pathlist.count())
        return 0;
    switch (m_searchtype) {
        case Settings : //设置项，返回功能点名
            return FileUtils::getSettingName(m_pathlist.at(index));
        case Contents:
        case Dirs :
        case Files : //文件，返回文件名
            return FileUtils::getFileName(m_pathlist.at(index));
        case Apps : //应用，返回应用名
            return FileUtils::getAppName(m_pathlist.at(index));
        case Best : //最佳匹配，含全部类型，需要自己判断，返回不同类型的名称
//            return m_pathlist.at(index);
            if (m_pathlist.at(index).endsWith(".desktop")) {
                return FileUtils::getAppName(m_pathlist.at(index));
            } else if (QFileInfo(m_pathlist.at(index)).isFile() || QFileInfo(m_pathlist.at(index)).isDir()) {
                return FileUtils::getFileName(m_pathlist.at(index));
            } else {
                return FileUtils::getSettingName(m_pathlist.at(index));
            }
        default:
            return m_pathlist.at(index);
    }
}

/**
 * @brief SearchItem::setSearchList 存入搜索结果列表
 * @param type 搜索类型
 * @param searchResult 搜索结果
 */
void SearchItem::setSearchList(const int& type, const QStringList& searchResult) {
    m_searchtype = type;
    m_pathlist = searchResult;
}

/**
 * @brief SearchItem::removeItem
 */
void SearchItem::removeItem(QString path) {
    m_pathlist.removeOne(path);
}

int SearchItem::getCurrentSize() {
    return m_pathlist.length();
}

void SearchItem::clear()
{
    m_pathlist.clear();
}
