#include "search-list-view.h"
#include <QDebug>
#include <QFileInfo>

SearchListView::SearchListView(QWidget * parent, const QStringList& list, const int& type, const QString& keyword) : QTreeView(parent)
{
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    m_model = new SearchItemModel;
    m_item = new SearchItem;
    m_item->setSearchList(type, list);
    m_model->setItem(m_item);
    this->setModel(m_model);
    this->setHeaderHidden(true);
    this->setColumnWidth(0, 20);
    this->setColumnWidth(1, 80);
    rowheight = this->rowHeight(this->model()->index(0,1, QModelIndex())) + 1;
    this->setFixedHeight(list.count() * rowheight + 2);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setAutoFillBackground(false);
    this->setStyleSheet("QWidget{background:transparent;}");
    m_styleDelegate = new HighlightItemDelegate();
    m_styleDelegate->setSearchKeyword(keyword);
    this->setItemDelegate(m_styleDelegate);

    m_type = type;
    connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, this, [ = ]() {
        Q_EMIT this->currentRowChanged(getCurrentType(), m_item->m_pathlist.at(this->currentIndex().row()));
    });
}

SearchListView::~SearchListView()
{
    if (m_model) {
        delete m_model;
        m_model = NULL;
    }
    if (m_item) {
        delete m_item;
        m_item = NULL;
    }
}

/**
 * @brief SearchListView::appendItem
 */
void SearchListView::appendItem(QString path) {
    m_model->appendItem(path);
    rowheight = this->rowHeight(this->model()->index(0,1, QModelIndex())) + 1;
    this->setFixedHeight(m_item->getCurrentSize() * rowheight + 3);
}

/**
 * @brief SearchListView::removeItem
 */
void SearchListView::removeItem(QString path) {
    m_model->removeItem(path);
}

void SearchListView::clear()
{
    m_model->clear();
    this->setFixedHeight(0);
    this->isHidden = true;
}

//获取当前选项所属搜索类型
int SearchListView::getCurrentType() {
    switch (m_type) {
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
    default: //All或者Best的情况，需要自己判断文件类型
        return getResType(m_item->m_pathlist.at(this->currentIndex().row()));
        break;
    }
}

/**
 * @brief SearchListView::getResType 根据路径返回文件类型
 * @param path 路径
 * @return
 */
int SearchListView::getResType(const QString& path) {
    if (path.endsWith(".desktop")) {
//        qDebug()<<"qDebug: One row selected, its path is "<<path<<". Its type is application.";
        return SearchListView::ResType::App;
    } else if (QFileInfo(path).isFile()) {
//        qDebug()<<"qDebug: One row selected, its path is "<<path<<". Its type is file.";
        return SearchListView::ResType::File;
    } else if (QFileInfo(path).isDir()) {
//        qDebug()<<"qDebug: One row selected, its path is "<<path<<". Its type is dir.";
        return SearchListView::ResType::Dir;
    } else {
//        qDebug()<<"qDebug: One row selected, its path is "<<path<<". Its type is setting.";
        return SearchListView::ResType::Setting;
    }
}

/**
 * @brief SearchListView::clearSelection 取消当前选项的选中
 */
void SearchListView::clearSelection() {
    this->selectionModel()->clearSelection();
}
