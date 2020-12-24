#include "search-list-view.h"
#include <QDebug>

SearchListView::SearchListView(QWidget * parent, const QStringList& list, int type) : QTreeView(parent)
{
    m_model = new SearchItemModel;
    m_item = new SearchItem;
    m_item->setSearchList(type, list);
    m_model->setItem(m_item);
    this->setModel(m_model);
    this->setHeaderHidden(true);
    this->setColumnWidth(0, 20);
    this->setColumnWidth(1, 80);
    this->setFixedHeight(list.count() * 47 + 2);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setAutoFillBackground(false);
    this->setStyleSheet("QWidget{background:transparent;}");
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
