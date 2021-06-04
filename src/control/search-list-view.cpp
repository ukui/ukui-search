/*
 *
 * Copyright (C) 2020, KylinSoft Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: zhangjiaping <zhangjiaping@kylinos.cn>
 *
 */
#include "search-list-view.h"
#include <QDebug>
#include <QFileInfo>

using namespace Zeeker;
SearchListView::SearchListView(QWidget * parent, const QStringList& list, const int& type) : QTreeView(parent) {
    CustomStyle * style = new CustomStyle(GlobalSettings::getInstance()->getValue(STYLE_NAME_KEY).toString());
    this->setStyle(style);
    this->setFrameShape(QFrame::NoFrame);
    this->viewport()->setAutoFillBackground(false);
    setRootIsDecorated(false);

    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    m_model = new SearchItemModel(this);
    m_item = new SearchItem;
    m_item->setSearchList(type, list);
    m_model->setItem(m_item);
    this->setModel(m_model);
    this->setHeaderHidden(true);
//    this->setColumnWidth(0, 20);
//    this->setColumnWidth(1, 80);
//    this->setFixedHeight(list.count() * rowheight + 4);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setAutoFillBackground(false);
    m_styleDelegate = new HighlightItemDelegate(this);
//    m_styleDelegate->setSearchKeyword(keyword);
    this->setItemDelegate(m_styleDelegate);

    m_type = type;
    connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, this, [ = ](const QItemSelection & selected, const QItemSelection & deselected) {
        Q_EMIT this->currentRowChanged(this, getCurrentType(), m_item->m_pathlist.at(this->currentIndex().row()));
        m_isSelected = true;
        if(!selected.isEmpty()) {
            QRegion region = visualRegionForSelection(selected);
            QRect rect = region.boundingRect();
            Q_EMIT this->currentSelectPos(mapToParent(rect.topLeft()));
        }
    });

    connect(this, &SearchListView::activated, this, [ = ](const QModelIndex & index) {
        Q_EMIT this->onRowDoubleClicked(this, getCurrentType(), m_item->m_pathlist.at(index.row()));
    });
}

SearchListView::~SearchListView() {
    if(m_model) {
        delete m_model;
        m_model = NULL;
    }
    if(m_item) {
        delete m_item;
        m_item = NULL;
    }
}

/**
 * @brief SearchListView::appendItem 单个添加
 */
void SearchListView::appendItem(QString path) {
    m_model->appendItem(path);
    rowheight = this->rowHeight(this->model()->index(0, 0, QModelIndex()));
    this->setFixedHeight(m_item->getCurrentSize() * rowheight);
}

/**
 * @brief SearchListView::setList 设置整个列表
 */
void SearchListView::setList(QStringList list) {
    QModelIndex index = this->currentIndex();
    m_model->setList(list);
    if(index.row() >= 0 && index.row() < list.length() && m_isSelected) {
        this->blockSignals(true);
        this->setCurrentIndex(index);
        this->blockSignals(false);
    }
    rowheight = this->rowHeight(this->model()->index(0, 0, QModelIndex()));
    this->setFixedHeight(m_item->getCurrentSize() * rowheight);
}

void SearchListView::setAppList(const QStringList &pathlist, const QStringList &iconlist) {
    m_model->setAppList(pathlist, iconlist);
}

void SearchListView::appendBestItem(const QPair<int, QString> &pair) {
    m_model->appendBestItem(pair);
}

/**
 * @brief SearchListView::removeItem
 */
void SearchListView::removeItem(QString path) {
    m_model->removeItem(path);
}

void SearchListView::clear() {
    this->blockSignals(true);
    this->clearSelection();
    this->blockSignals(false);
    m_model->clear();
    this->setFixedHeight(0);
    this->isHidden = true;
}

/**
 * @brief SearchListView::refresh 重新绘制当前列表
 */
void SearchListView::refresh()
{
    rowheight = this->rowHeight(this->model()->index(0, 0, QModelIndex()));
    this->setFixedHeight(m_item->getCurrentSize() * rowheight);
}

/**
 * @brief SearchListView::setKeyword 设置关键词
 * @param keyword 关键词
 */
void SearchListView::setKeyword(QString keyword) {
    m_styleDelegate->setSearchKeyword(keyword);
}

/**
 * @brief SearchListView::getType 获取此列表类型
 * @return
 */
int SearchListView::getType() {
    return m_type;
}

/**
 * @brief SearchListView::getLength 获取当前显示的列表项数量
 * @return
 */
int SearchListView::getLength() {
    return m_item->getCurrentSize();
}

void SearchListView::mousePressEvent(QMouseEvent *event) {
    if(event->button() == Qt::LeftButton) {
        Q_EMIT mousePressed();
    }
    QTreeView::mousePressEvent(event);
}

//获取当前选项所属搜索类型
int SearchListView::getCurrentType() {
    switch(m_type) {
    case SearchItem::SearchType::Apps :
//        qDebug()<<"qDebug: One row selected, its type is application.";
        return ResType::App;
    case SearchItem::SearchType::Files:
//        qDebug()<<"qDebug: One row selected, its type is file.";
        return ResType::File;
    case SearchItem::SearchType::Settings:
//        qDebug()<<"qDebug: One row selected, its type is setting.";
        return ResType::Setting;
    case SearchItem::SearchType::Dirs:
//        qDebug()<<"qDebug: One row selected, its type is dir.";
        return ResType::Dir;
    case SearchItem::SearchType::Contents:
//        qDebug()<<"qDebug: One row selected, its type is content.";
        return ResType::Content;
    case SearchItem::SearchType::Web:
        return ResType::Web;
    default: //All或者Best的情况，需要自己判断文件类型
//        return getResType(m_item->m_pathlist.at(this->currentIndex().row()));
        return ResType::Best;
        break;
    }
}

/**
 * @brief SearchListView::getResType 根据路径返回文件类型
 * @param path 路径
 * @return
 */
int SearchListView::getResType(const QString& path) {
    if(path.endsWith(".desktop")) {
        return SearchListView::ResType::App;
    } else if(QFileInfo(path).isFile()) {
        return SearchListView::ResType::Best;
    } else if(QFileInfo(path).isDir()) {
        return SearchListView::ResType::Dir;
    } else {
        return SearchListView::ResType::Setting;
    }
}

/**
 * @brief SearchListView::clearSelection 取消当前选项的选中
 */
void SearchListView::clearSelection() {
    this->selectionModel()->clearSelection();
    m_isSelected = false;
}

CustomStyle::CustomStyle(const QString &proxyStyleName, QObject *parent) : QProxyStyle(proxyStyleName)
{
}

/**
 * @brief CustomStyle::sizeFromContents 重写此方法以加高行高
 * @param type
 * @param option
 * @param contentsSize
 * @param widget
 * @return
 */
QSize CustomStyle::sizeFromContents(QStyle::ContentsType type, const QStyleOption *option, const QSize &contentsSize, const QWidget *widget) const
{
    switch (type) {
        case CT_ItemViewItem: {
            QSize size(0, GlobalSettings::getInstance()->getValue(FONT_SIZE_KEY).toDouble() * 2 + 8);
            return size;
        } break;
        default: break;
    }
    return QProxyStyle::sizeFromContents(type, option, contentsSize, widget);
}
