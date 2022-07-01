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
            return m_list.at(row).icon.name();

        case SearchResultModel::DescriptionRole:
            return generateDesc(m_list.at(row));

        default:
            return {};
    }
}

QString SearchResultModel::generateDesc(const SearchPluginIface::ResultInfo& data)
{
    QVector<SearchPluginIface::DescriptionInfo> description = data.description;

    QStringList objs;
    for (const auto &item: description) {
        objs.append(R"({"k":")" + item.key + R"(","v":")" + item.value.split("\n").first() + R"("})");
    }

    QString keys = QString(R"("keys":[%1])").arg(objs.join(","));
    QString desc = QString(R"({"type":%1,"actionKey":"%2",)").arg(data.type).arg(data.actionKey);

    desc.push_back(keys);
    desc.push_back("}");

    return desc;
}

SearchResultModel::~SearchResultModel()
{

}

void SearchResultModel::insertData(const SearchPluginIface::ResultInfo &data)
{
    int begin = m_list.count();
    int limit = m_expanded ? m_maxRowCount : m_minRowCount;

    m_list.push_back(data);
    //如果数据已经超过当前界面允许显示的最大条数后，不再给view发送数据添加信号
    if (begin < limit) {
        beginInsertRows(QModelIndex(), begin, begin);
        endInsertRows();
    }

    setCanExpand(m_list.count() > m_minRowCount);
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

    setCanExpand(false);
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

void SearchResultModel::setCanExpand(bool status)
{
    if (m_canExpand != status) {
        m_canExpand = status;
        Q_EMIT canExpand(this, m_canExpand);
    }
}
