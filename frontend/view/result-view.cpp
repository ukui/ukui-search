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

    //NEW_TODO
    m_showMoreLabel = new ShowMoreLabel(this);
    m_showMoreLabel->setFixedHeight(UNFOLD_LABEL_HEIGHT);

    m_mainLyt->addWidget(m_titleLabel);
    m_mainLyt->addWidget(m_resultView);
    m_mainLyt->addWidget(m_showMoreLabel);
    this->setFixedHeight(m_resultView->height() + TITLE_HEIGHT + UNFOLD_LABEL_HEIGHT);
}

void ResultWidget::initConnections()
{
    connect(this, &ResultWidget::startSearch, m_resultView, &ResultView::startSearch);
    connect(this, &ResultWidget::stopSearch, m_resultView, &ResultView::stopSearch);
    connect(m_resultView, &ResultView::currentRowChanged, this, &ResultWidget::currentRowChanged);
    connect(this, &ResultWidget::clearSelectedRow, m_resultView, &ResultView::clearSelectedRow);
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

void ResultView::initConnections()
{
    connect(this, &ResultView::startSearch, m_model, &SearchResultModel::startSearch);
    connect(this, &ResultView::stopSearch, m_model, &SearchResultModel::stopSearch);
    connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, this, [ = ](const QItemSelection & selected, const QItemSelection & deselected) {
        //NEW_TODO 处理选中事件
        m_is_selected = true;
        Q_EMIT this->currentRowChanged(m_plugin_id, m_model->getInfo(this->currentIndex()));
        m_is_selected = false;
        if(!selected.isEmpty()) {
            QRegion region = visualRegionForSelection(selected);
            QRect rect = region.boundingRect();
//            Q_EMIT this->currentSelectPos(mapToParent(rect.topLeft()));
        }
    });

    connect(this, &ResultView::activated, this, [ = ](const QModelIndex & index) {
        //NEW_TODO 处理双击打开事件
    });
}
