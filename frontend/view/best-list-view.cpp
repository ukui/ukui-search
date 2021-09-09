#include "best-list-view.h"
#define MAIN_MARGINS 0,0,0,0
#define MAIN_SPACING 0
#define TITLE_HEIGHT 30
#define UNFOLD_LABEL_HEIGHT 30

using namespace Zeeker;
BestListView::BestListView(QWidget *parent) : QTreeView(parent)
{
    this->setFrameShape(QFrame::NoFrame);
    this->viewport()->setAutoFillBackground(false);
    this->setRootIsDecorated(false);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setHeaderHidden(true);
    m_model = new BestListModel(this);
    this->setModel(m_model);
    initConnections();
    m_style_delegate = new ResultViewDelegate(this);
    this->setItemDelegate(m_style_delegate);
}

bool BestListView::isSelected()
{
    return m_is_selected;
}

int BestListView::showHeight()
{
    int height;
    int rowheight = this->rowHeight(this->model()->index(0, 0, QModelIndex()));
    if (this->isExpanded()) {
        height = m_count * rowheight;
    } else {
        int show_count = m_count > NUM_LIMIT_SHOWN_DEFAULT ? NUM_LIMIT_SHOWN_DEFAULT : m_count;
        height = show_count * rowheight;
    }
    return height;
}

int BestListView::getResultNum()
{
    return m_count;
}

QModelIndex BestListView::getModlIndex(int row, int column)
{
    return this->m_model->index(row, column);
}

SearchPluginIface::ResultInfo BestListView::getIndexResultInfo(QModelIndex &index)
{
    return this->m_model->getInfo(index);
}

const QString BestListView::getPluginInfo(const QModelIndex& index)
{
    return this->m_model->getPluginInfo(index);
}

int BestListView::getResultHeight()
{
    return this->rowHeight(this->model()->index(0, 0, QModelIndex()));
}

void BestListView::clearSelectedRow()
{
    if (!m_is_selected) {
        this->blockSignals(true);
        this->clearSelection();
        this->blockSignals(false);
    }
}

/**
 * @brief BestListView::onRowDoubleClickedSlot 处理列表中的双击打开事件
 * @param index 点击的条目
 */
void BestListView::onRowDoubleClickedSlot(const QModelIndex &index)
{
    const SearchPluginIface::ResultInfo &info = m_model->getInfo(index);
    QString plugin_id = m_model->getPluginInfo(index);;
    SearchPluginIface *plugin = SearchPluginManager::getInstance()->getPlugin(plugin_id);
    try {
        if (plugin) {
            if (!info.actionKey.isEmpty()) {
                plugin->openAction(0, info.actionKey, info.type);
            } else {
                throw -2;
            }
        } else {
            throw -1;
        }
    } catch(int e) {
        qWarning()<<"Open failed, reason="<<e;
    }
}

/**
 * @brief BestListView::onRowSelectedSlot 处理列表项选中事件
 * @param selected
 * @param deselected
 */
void BestListView::onRowSelectedSlot(const QModelIndex &index)
{
    m_is_selected = true;
    Q_EMIT this->currentRowChanged(m_model->getPluginInfo(index), m_model->getInfo(index));
}

void BestListView::onItemListChanged(const int &count)
{
    m_count = count;
    Q_EMIT this->listLengthChanged(count);
}

void BestListView::setExpanded(const bool &is_expanded)
{
    QModelIndex index = this->currentIndex();
    m_model->setExpanded(is_expanded);
    this->setCurrentIndex(index);
}

const bool &BestListView::isExpanded()
{
    return m_model->isExpanded();
}

/**
 * @brief BestListView::onMenuTriggered 点击右键菜单的槽函数
 * @param action
 */
void BestListView::onMenuTriggered(QAction *action)
{
    //NEW_TODO 接口调整后需要修改
//    SearchPluginIface *plugin = SearchPluginManager::getInstance()->getPlugin(m_plugin_id);
//    if (plugin) {
////        plugin->openAction(action->text(), m_model->getKey(this->currentIndex()));
//    } else {
//        qWarning()<<"Get plugin failed!";
//    }
}

void BestListView::mousePressEvent(QMouseEvent *event)
{
    m_tmpCurrentIndex = this->currentIndex();
    m_tmpMousePressIndex = indexAt(event->pos());
    if (m_tmpMousePressIndex.isValid() and m_tmpCurrentIndex != m_tmpMousePressIndex) {
        Q_EMIT this->clicked(m_tmpMousePressIndex);
    }

    return QTreeView::mousePressEvent(event);
}

void BestListView::mouseReleaseEvent(QMouseEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if (index.isValid()) {
        Q_EMIT this->clicked(index);
    } else {
        Q_EMIT this->clicked(this->currentIndex());
    }
    return QTreeView::mouseReleaseEvent(event);
}

void BestListView::mouseMoveEvent(QMouseEvent *event)
{
   m_tmpCurrentIndex = this->currentIndex();
   m_tmpMousePressIndex = indexAt(event->pos());
   if (m_tmpMousePressIndex.isValid() and m_tmpCurrentIndex != m_tmpMousePressIndex) {
       Q_EMIT this->clicked(m_tmpMousePressIndex);
   }
    return QTreeView::mouseMoveEvent(event);
}

void BestListView::initConnections()
{
    connect(this, &BestListView::startSearch, [ = ](const QString &keyword) {
        qDebug() << "==========start search!";
        m_style_delegate->setSearchKeyword(keyword);
        m_model->startSearch(keyword);
    });
    connect(this, &BestListView::startSearch, m_model, &BestListModel::startSearch);
    connect(this, &BestListView::clicked, this, &BestListView::onRowSelectedSlot);
    connect(this, &BestListView::activated, this, &BestListView::onRowDoubleClickedSlot);
    connect(m_model, &BestListModel::itemListChanged, this, &BestListView::onItemListChanged);
    connect(this, &BestListView::sendBestListData, this, [=] (const QString &plugin, const SearchPluginIface::ResultInfo&info) {
        QModelIndex index = this->currentIndex();
        this->m_model->appendInfo(plugin, info);
        if (index.isValid()) {
            this->setCurrentIndex(index);
        }
    });
}

BestListWidget::BestListWidget(QWidget *parent) : QWidget(parent)
{
    this->initUi();
    initConnections();
}

QString BestListWidget::getWidgetName()
{
    return m_titleLabel->text();
}

void BestListWidget::setEnabled(const bool &enabled)
{
    m_enabled = enabled;
}

void BestListWidget::clearResult()
{
    this->setVisible(false);
    this->setFixedHeight(0);
}

int BestListWidget::getResultNum()
{
    return m_bestListView->getResultNum();
}

void BestListWidget::setResultSelection(const QModelIndex &index)
{
    this->m_bestListView->setCurrentIndex(index);
}

void BestListWidget::clearResultSelection()
{
    //this->m_bestListView->selectionModel()->clearSelection();
    this->m_bestListView->setCurrentIndex(QModelIndex());
}

QModelIndex BestListWidget::getModlIndex(int row, int column)
{
    return this->m_bestListView->getModlIndex(row, column);
}

void BestListWidget::activateIndex()
{
    this->m_bestListView->onRowDoubleClickedSlot(this->m_bestListView->currentIndex());
}

QModelIndex BestListWidget::getCurrentSelection()
{
    return this->m_bestListView->currentIndex();
}

bool BestListWidget::getExpandState()
{
    return m_bestListView->isExpanded();
}

SearchPluginIface::ResultInfo BestListWidget::getIndexResultInfo(QModelIndex &index)
{
    return m_bestListView->getIndexResultInfo(index);
}

const QString BestListWidget::getPluginInfo(const QModelIndex&index)
{
    return this->m_bestListView->getPluginInfo(index);
}

int BestListWidget::getResultHeight()
{
    return this->m_bestListView->getResultHeight();
}
/**
 * @brief BestListWidget::expandListSlot 展开列表的槽函数
 */
void BestListWidget::expandListSlot()
{
    qWarning()<<"List will be expanded!";
    m_bestListView->setExpanded(true);
}

/**
 * @brief BestListWidget::reduceListSlot 收起列表的槽函数
 */
void BestListWidget::reduceListSlot()
{
    qWarning()<<"List will be reduced!";
    m_bestListView->setExpanded(false);
}

/**
 * @brief BestListWidget::onListLengthChanged 响应列表长度改变的槽函数
 */
void BestListWidget::onListLengthChanged(const int &length)
{
    this->setVisible(length > 0);
    int whole_height = this->isVisible() ? (m_bestListView->showHeight() + TITLE_HEIGHT) : 0;
    this->setFixedHeight(whole_height);
    Q_EMIT this->sizeChanged();
}

void BestListWidget::initUi()
{
    m_mainLyt = new QVBoxLayout(this);
    this->setLayout(m_mainLyt);
    m_mainLyt->setContentsMargins(MAIN_MARGINS);
    m_mainLyt->setSpacing(MAIN_SPACING);

    m_titleLabel = new TitleLabel(this);
    m_titleLabel->setText(tr("Best Matches"));
    m_titleLabel->setFixedHeight(TITLE_HEIGHT);

    m_bestListView = new BestListView(this);

    m_mainLyt->addWidget(m_titleLabel);
    m_mainLyt->addWidget(m_bestListView);
    this->setFixedHeight(m_bestListView->height() + TITLE_HEIGHT);
    this->setFixedWidth(656);
}

void BestListWidget::initConnections()
{
    connect(this, &BestListWidget::startSearch, m_bestListView, &BestListView::startSearch);
    connect(this, &BestListWidget::startSearch, m_titleLabel, &TitleLabel::startSearch);
    connect(this, &BestListWidget::startSearch, m_titleLabel, &TitleLabel::startSearch);
    connect(this, &BestListWidget::stopSearch, m_titleLabel, &TitleLabel::stopSearch);
    connect(this, &BestListWidget::sendBestListData, m_bestListView, &BestListView::sendBestListData);
    connect(m_bestListView, &BestListView::currentRowChanged, this, &BestListWidget::currentRowChanged);
    connect(this, &BestListWidget::clearSelectedRow, m_bestListView, &BestListView::clearSelectedRow);
    connect(m_titleLabel, &TitleLabel::showMoreClicked, this, &BestListWidget::expandListSlot);
    connect(m_titleLabel, &TitleLabel::retractClicked, this, &BestListWidget::reduceListSlot);
    connect(m_bestListView, &BestListView::listLengthChanged, this, &BestListWidget::onListLengthChanged);
    connect(m_bestListView, &BestListView::listLengthChanged, m_titleLabel, &TitleLabel::onListLengthChanged);
    connect(m_bestListView, &BestListView::clicked, this, &BestListWidget::rowClicked);
    connect(qApp, &QApplication::paletteChanged, this, [ = ]() {
        int whole_height = this->isVisible() ? m_bestListView->showHeight() + TITLE_HEIGHT : 0;
        this->setFixedHeight(whole_height);
        Q_EMIT this->sizeChanged();
    });
}
