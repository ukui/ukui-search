#include "result-view.h"
#define MAIN_MARGINS 0,0,0,0
#define MAIN_SPACING 0
#define TITLE_HEIGHT 30
#define UNFOLD_LABEL_HEIGHT 30

using namespace Zeeker;
ResultWidget::ResultWidget(const QString &plugin_id, QWidget *parent) : QWidget(parent)
{
    m_plugin_id = plugin_id;
    this->initUi();
    initConnections();
}

QString ResultWidget::pluginId()
{
    return m_plugin_id;
}

void ResultWidget::setEnabled(const bool &enabled)
{
    m_enabled = enabled;
}

/**
 * @brief ResultWidget::expandListSlot 展开列表的槽函数
 */
void ResultWidget::expandListSlot()
{
    qWarning()<<"List will be expanded!";
    m_resultView->setExpanded(true);
}

/**
 * @brief ResultWidget::reduceListSlot 收起列表的槽函数
 */
void ResultWidget::reduceListSlot()
{
    qWarning()<<"List will be reduced!";
    m_resultView->setExpanded(false);
}

/**
 * @brief ResultWidget::onListLengthChanged 响应列表长度改变的槽函数
 */
void ResultWidget::onListLengthChanged(const int &length)
{
    this->setVisible(length > 0);
    m_showMoreLabel->setVisible(length >= NUM_LIMIT_SHOWN_DEFAULT);
    int show_more_height = m_showMoreLabel->isVisible() ? UNFOLD_LABEL_HEIGHT : 0;
    int whole_height = this->isVisible() ? m_resultView->showHeight() + TITLE_HEIGHT + show_more_height : 0;
    this->setFixedHeight(whole_height);
    Q_EMIT this->sizeChanged();
}

void ResultWidget::initUi()
{
    m_mainLyt = new QVBoxLayout(this);
    this->setLayout(m_mainLyt);
    m_mainLyt->setContentsMargins(MAIN_MARGINS);
    m_mainLyt->setSpacing(MAIN_SPACING);

    m_titleLabel = new TitleLabel(this);
    m_titleLabel->setText(m_plugin_id);
    m_titleLabel->setFixedHeight(TITLE_HEIGHT);

    m_resultView = new ResultView(m_plugin_id, this);

    m_showMoreLabel = new ShowMoreLabel(this);
    m_showMoreLabel->setFixedHeight(UNFOLD_LABEL_HEIGHT);
    m_showMoreLabel->hide();

    m_mainLyt->addWidget(m_titleLabel);
    m_mainLyt->addWidget(m_resultView);
    m_mainLyt->addWidget(m_showMoreLabel);
    this->setFixedHeight(m_resultView->height() + TITLE_HEIGHT);
}

void ResultWidget::initConnections()
{
    connect(this, &ResultWidget::startSearch, m_resultView, &ResultView::startSearch);
    connect(this, &ResultWidget::startSearch, this, [ = ]() {
        m_showMoreLabel->resetLabel();
    });
    connect(this, &ResultWidget::stopSearch, m_resultView, &ResultView::stopSearch);
    connect(this, &ResultWidget::stopSearch, this, [ = ]() {
        m_showMoreLabel->resetLabel();
        m_resultView->setExpanded(false);
    });
    connect(m_resultView, &ResultView::currentRowChanged, this, &ResultWidget::currentRowChanged);
    connect(this, &ResultWidget::clearSelectedRow, m_resultView, &ResultView::clearSelectedRow);
    connect(m_showMoreLabel, &ShowMoreLabel::showMoreClicked, this, &ResultWidget::expandListSlot);
    connect(m_showMoreLabel, &ShowMoreLabel::retractClicked, this, &ResultWidget::reduceListSlot);
    connect(m_resultView, &ResultView::listLengthChanged, this, &ResultWidget::onListLengthChanged);
    connect(m_resultView, &ResultView::rowClicked, this, &ResultWidget::rowClicked);
    connect(qApp, &QApplication::paletteChanged, this, [ = ]() {
        int show_more_height = m_showMoreLabel->isVisible() ? UNFOLD_LABEL_HEIGHT : 0;
        int whole_height = this->isVisible() ? m_resultView->showHeight() + TITLE_HEIGHT + show_more_height : 0;
        this->setFixedHeight(whole_height);
        Q_EMIT this->sizeChanged();
    });
}

ResultView::ResultView(const QString &plugin_id, QWidget *parent) : QTreeView(parent)
{
    this->setFrameShape(QFrame::NoFrame);
    this->viewport()->setAutoFillBackground(false);
    this->setRootIsDecorated(false);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setHeaderHidden(true);
    m_model = new SearchResultModel(plugin_id);
    this->setModel(m_model);
    initConnections();
    m_plugin_id = plugin_id;
    m_style_delegate = new ResultViewDelegate(this);
    this->setItemDelegate(m_style_delegate);
}

bool ResultView::isSelected()
{
    return m_is_selected;
}

int ResultView::showHeight()
{
    int height;
    int rowheight = this->rowHeight(this->model()->index(0, 0, QModelIndex())) + 1;
    if (this->isExpanded()) {
        height = m_count * rowheight;
    } else {
        int show_count = m_count > NUM_LIMIT_SHOWN_DEFAULT ? NUM_LIMIT_SHOWN_DEFAULT : m_count;
        height = show_count * rowheight;
    }
    return height;
}

void ResultView::clearSelectedRow()
{
    if (!m_is_selected) {
        this->blockSignals(true);
        this->clearSelection();
        this->blockSignals(false);
    }
}

/**
 * @brief ResultView::onRowDoubleClickedSlot 处理列表中的双击打开事件
 * @param index 点击的条目
 */
void ResultView::onRowDoubleClickedSlot(const QModelIndex &index)
{
    const SearchPluginIface::ResultInfo &info = m_model->getInfo(index);
    SearchPluginIface *plugin = SearchPluginManager::getInstance()->getPlugin(m_plugin_id);
    try {
        if (plugin) {
            if (!info.actionList.isEmpty()) {
                plugin->openAction(info.actionList.at(0), info.key);
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
 * @brief ResultView::onRowSelectedSlot 处理列表项选中事件
 * @param selected
 * @param deselected
 */
void ResultView::onRowSelectedSlot(const QItemSelection &selected, const QItemSelection &deselected)
{
    //NEW_TODO
    m_is_selected = true;
    Q_EMIT this->currentRowChanged(m_plugin_id, m_model->getInfo(this->currentIndex()));
    m_is_selected = false;
    if(!selected.isEmpty()) {
        QRegion region = visualRegionForSelection(selected);
        QRect rect = region.boundingRect();
//            Q_EMIT this->currentSelectPos(mapToParent(rect.topLeft()));
    }
}

void ResultView::onItemListChanged(const int &count)
{
    m_count = count;
    Q_EMIT this->listLengthChanged(count);
}

void ResultView::setExpanded(const bool &is_expanded)
{
    m_model->setExpanded(is_expanded);
}

const bool &ResultView::isExpanded()
{
    return m_model->isExpanded();
}

/**
 * @brief ResultView::onMenuTriggered 点击右键菜单的槽函数
 * @param action
 */
void ResultView::onMenuTriggered(QAction *action)
{
    //NEW_TODO 接口调整后需要修改
    SearchPluginIface *plugin = SearchPluginManager::getInstance()->getPlugin(m_plugin_id);
    if (plugin) {
        plugin->openAction(action->text(), m_model->getKey(this->currentIndex()));
    } else {
        qWarning()<<"Get plugin failed!";
    }
}

void ResultView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        //加一点点延时，等待列表先被选中
        QTimer::singleShot(10, this, [ = ] {
            QMenu * menu = new QMenu(this);
            QStringList actions = m_model->getActions(this->currentIndex());
            Q_FOREACH (QString action, actions) {
                menu->addAction(new QAction(action, this));
            }
            menu->move(cursor().pos());
            menu->show();
            connect(menu, &QMenu::triggered, this, &ResultView::onMenuTriggered);
        });
    }
    Q_EMIT this->rowClicked();
    return QTreeView::mousePressEvent(event);
}

void ResultView::initConnections()
{
//    connect(this, &ResultView::startSearch, m_model, &SearchResultModel::startSearch);
    connect(this, &ResultView::startSearch, [ = ](const QString &keyword) {
        m_style_delegate->setSearchKeyword(keyword);
        m_model->startSearch(keyword);
    });
    connect(this, &ResultView::stopSearch, m_model, &SearchResultModel::stopSearch);
    connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ResultView::onRowSelectedSlot);
    connect(this, &ResultView::activated, this, &ResultView::onRowDoubleClickedSlot);
    connect(m_model, &SearchResultModel::itemListChanged, this, &ResultView::onItemListChanged);
}
