#include "result-view.h"
#define MAIN_MARGINS 0,0,0,0
#define MAIN_SPACING 0
#define TITLE_HEIGHT 30
#define UNFOLD_LABEL_HEIGHT 24

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
//NEW_TODO
    qWarning()<<"List will be expanded!";
}

/**
 * @brief ResultWidget::reduceListSlot 收起列表的槽函数
 */
void ResultWidget::reduceListSlot()
{
//NEW_TODO
    qWarning()<<"List will be reduced!";
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

    //NEW_TODO 当列表条目大于n？时显示
    m_showMoreLabel = new ShowMoreLabel(this);
    m_showMoreLabel->setFixedHeight(UNFOLD_LABEL_HEIGHT);
//    m_showMoreLabel->hide();

    m_mainLyt->addWidget(m_titleLabel);
    m_mainLyt->addWidget(m_resultView);
    m_mainLyt->addWidget(m_showMoreLabel);
    this->setFixedHeight(m_resultView->height() + TITLE_HEIGHT + UNFOLD_LABEL_HEIGHT);
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
    });
    connect(m_resultView, &ResultView::currentRowChanged, this, &ResultWidget::currentRowChanged);
    connect(this, &ResultWidget::clearSelectedRow, m_resultView, &ResultView::clearSelectedRow);
    connect(m_showMoreLabel, &ShowMoreLabel::showMoreClicked, this, &ResultWidget::expandListSlot);
    connect(m_showMoreLabel, &ShowMoreLabel::retractClicked, this, &ResultWidget::reduceListSlot);
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
}
