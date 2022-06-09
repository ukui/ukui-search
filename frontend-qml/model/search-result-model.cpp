//
// Created by hxf on 22-5-26.
//

#include "search-result-model.h"

#include <QDebug>
#include <QtMath>

using namespace UkuiSearch;

SearchResultModel::SearchResultModel(QObject *parent) : QAbstractListModel(parent)
{
    m_roleNamesHash[SearchResultModel::NameRole] = "name";
    m_roleNamesHash[SearchResultModel::IconRole] = "icon";
    m_roleNamesHash[SearchResultModel::IconNameRole] = "iconName";
    m_roleNamesHash[SearchResultModel::DescriptionRole] = "description";
    m_roleNamesHash[Qt::ToolTipRole] = "tooltip";
}

int SearchResultModel::rowCount(const QModelIndex &parent) const
{
    int rowCount = virtualRowCount();
    qDebug() << "SearchResultModel::rowCount" << rowCount;

    return rowCount;
}

QHash<int, QByteArray> SearchResultModel::roleNames() const
{
    return m_roleNamesHash;
}

QVariant SearchResultModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int count = virtualRowCount();
    if (row < 0 || row >= count) {
        return {};
    }

    switch (role) {
        case SearchResultModel::NameRole:
            return m_list.at(row).name;

        case Qt::ToolTipRole:
            return m_list.at(row).name;

        case SearchResultModel::IconRole:
            return m_list.at(row).icon;

        case SearchResultModel::IconNameRole:
            qDebug() << "==============SearchResultModel::IconNameRol=" << m_list.at(row).name << m_list.at(row).icon.name();
            return m_list.at(row).icon.name();

        case SearchResultModel::DescriptionRole:
            if (m_list.at(row).description.isEmpty()) {
                return {""};
            }
            return m_list.at(row).description.first().value;

        default:
            return {};
    }
}

SearchResultModel::~SearchResultModel()
{

}

void SearchResultModel::insertData(const SearchPluginIface::ResultInfo &data)
{
    int begin = m_list.count();
    int limit = m_expanded ? m_maxRowCount : m_minRowCount;
    //如果数据已经超过当前界面允许显示的最大条数后，不再给view发送数据添加信号
    if (begin < limit) {
        beginInsertRows(QModelIndex(), begin, begin);
        m_list.push_back(data);
        endInsertRows();
        return;
    }
    m_list.push_back(data);
}

void SearchResultModel::clear()
{
    if (m_list.empty()) {
        return;
    }

    int count = virtualRowCount();
    qDebug() << "是否展开:" << m_expanded << "删除数量:" << count;
    //让view以为只删除了他显示的行数
    beginRemoveRows(QModelIndex(), 0, count);
    m_list.clear();
    endRemoveRows();
}

void SearchResultModel::expand()
{
    m_expanded = true;
    if (m_list.count() <= m_minRowCount) {
        return;
    }
    int insertRows = qMin(m_list.count(), m_maxRowCount);
    qDebug() << "insert count:" << insertRows;

    beginInsertRows(QModelIndex(), m_minRowCount, insertRows - 1);
    endInsertRows();
}

void SearchResultModel::collapse()
{
    m_expanded = false;
    if (m_list.count() <= m_minRowCount) {
        return;
    }

    int removeRows = qMin(m_list.count(), m_maxRowCount);
    qDebug() << "removeRows :" << m_minRowCount << removeRows;

    beginRemoveRows(QModelIndex(), m_minRowCount, removeRows - 1);
    endRemoveRows();
}

int SearchResultModel::virtualRowCount() const
{
    return m_expanded ? qMin(m_list.count(), m_maxRowCount) : qMin(m_list.count(), m_minRowCount);
}
