#include "result-view.h"
#define MAIN_MARGINS 0, 0, 0, 0
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

void ResultWidget::clearResult()
{
    this->setVisible(false);
    this->setFixedHeight(0);
}

int ResultWidget::getResultNum()
{
    return m_resultView->getResultNum();
}

void ResultWidget::setResultSelection(const QModelIndex &index)
{
    this->m_resultView->setCurrentIndex(index);
    this->m_resultView->setSelectedState(true);
}

void ResultWidget::clearResultSelection()
{
    this->m_resultView->setCurrentIndex(QModelIndex());
    this->m_resultView->setSelectedState(false);
}

QModelIndex ResultWidget::getModlIndex(int row, int column)
{
    return this->m_resultView->getModlIndex(row, column);
}

void ResultWidget::activateIndex()
{
    this->m_resultView->onRowDoubleClickedSlot(this->m_resultView->currentIndex());
}

QModelIndex ResultWidget::getCurrentSelection()
{
    return this->m_resultView->currentIndex();
}

bool ResultWidget::getExpandState()
{
    return m_resultView->isExpanded();
}

SearchPluginIface::ResultInfo ResultWidget::getIndexResultInfo(QModelIndex &index)
{
    return m_resultView->getIndexResultInfo(index);
}

int ResultWidget::getResultHeight()
{
    return this->m_resultView->getResultHeight();
}

void ResultWidget::resetTitleLabel()
{
    Q_EMIT this->m_titleLabel->lableReset();
}

bool ResultWidget::getSelectedState()
{
    return m_resultView->getSelectedState();
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
    if (length != 0) {
        Q_EMIT this->getResult();
    }
    this->setVisible(length > 0);
    int whole_height = this->isVisible() ? m_resultView->showHeight() + TITLE_HEIGHT : 0;
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

    m_mainLyt->addWidget(m_titleLabel);
    m_mainLyt->addWidget(m_resultView);
    this->setFixedHeight(m_resultView->height() + TITLE_HEIGHT);
    //全长720 - 左边距16 - 滚动条16 = 688
    this->setFixedWidth(688);
}

void ResultWidget::initConnections()
{
    connect(this, &ResultWidget::startSearch, m_resultView, &ResultView::startSearch);
    connect(this, &ResultWidget::startSearch, m_titleLabel, &TitleLabel::startSearch);
    connect(this, &ResultWidget::stopSearch, m_resultView, &ResultView::stopSearch);
    connect(this, &ResultWidget::stopSearch, m_titleLabel, &TitleLabel::stopSearch);
    connect(m_resultView, &ResultView::currentRowChanged, this, &ResultWidget::currentRowChanged);
    connect(m_titleLabel, &TitleLabel::showMoreClicked, this, &ResultWidget::expandListSlot);
    connect(m_titleLabel, &TitleLabel::showMoreClicked, this, &ResultWidget::showMoreClicked);
    connect(m_titleLabel, &TitleLabel::retractClicked, this, &ResultWidget::reduceListSlot);
    connect(m_titleLabel, &TitleLabel::retractClicked, this, &ResultWidget::retractClicked);
    connect(m_resultView, &ResultView::listLengthChanged, this, &ResultWidget::onListLengthChanged);
    connect(m_resultView, &ResultView::listLengthChanged, m_titleLabel, &TitleLabel::onListLengthChanged);
    connect(m_resultView, &ResultView::clicked, this, &ResultWidget::rowClicked);
    connect(qApp, &QApplication::paletteChanged, this, [ = ]() {
        int whole_height = this->isVisible() ? m_resultView->showHeight() + TITLE_HEIGHT : 0;
        this->setFixedHeight(whole_height);
        Q_EMIT this->sizeChanged();
    });
    connect(m_resultView, &ResultView::sendBestListData, this, &ResultWidget::sendBestListData);
    connect(m_resultView, &ResultView::lableReset, m_titleLabel, &TitleLabel::lableReset);
    connect(this, &ResultWidget::resizeWidth, this, [=] (const int &size) {
        //留出16像素给滚动条
        this->setFixedWidth(size - 16);
    });
}

ResultView::ResultView(const QString &plugin_id, QWidget *parent) : QTreeView(parent)
{
    setStyle(ResultItemStyle::getStyle());
    this->setFrameShape(QFrame::NoFrame);
    this->setIconSize(QSize(24, 24));
    this->viewport()->setAttribute(Qt::WA_AcceptTouchEvents);
//    this->viewport()->setAutoFillBackground(false);
    this->setRootIsDecorated(false);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setHeaderHidden(true);
    m_model = new SearchResultModel(plugin_id);
    this->setModel(m_model);
    initConnections();
    m_plugin_id = plugin_id;
    m_styleDelegate = new ResultViewDelegate(this);
    this->setItemDelegate(m_styleDelegate);
    m_touchTimer = new QTimer(this);
    m_touchTimer->setSingleShot(true);
    m_touchTimer->setInterval(100);
}

bool ResultView::isSelected()
{
    return m_is_selected;
}

int ResultView::showHeight()
{
    int height(0);
//    int rowheight = this->rowHeight(this->model()->index(0, 0, QModelIndex()));
//    if (this->isExpanded()) {
//        height = m_count * rowheight;
//    } else {
//        int show_count = m_count > NUM_LIMIT_SHOWN_DEFAULT ? NUM_LIMIT_SHOWN_DEFAULT : m_count;
//        height = show_count * rowheight;
//    }
    if (this->isExpanded()) {
        for (int i = 0; i<m_count; ++i) {
            height += this->rowHeight(this->model()->index(i, 0, QModelIndex()));
        }
    } else {
        int show_count = m_count > NUM_LIMIT_SHOWN_DEFAULT ? NUM_LIMIT_SHOWN_DEFAULT : m_count;
        for (int i = 0; i<show_count; ++i) {
            height += this->rowHeight(this->model()->index(i, 0, QModelIndex()));
        }
    }
    return height;
}

int ResultView::getResultNum()
{
    return m_count;
}

QModelIndex ResultView::getModlIndex(int row, int column)
{
    return this->m_model->index(row, column);
}

SearchPluginIface::ResultInfo ResultView::getIndexResultInfo(QModelIndex &index)
{
    return this->m_model->getInfo(index);
}

int ResultView::getResultHeight()
{
    return this->rowHeight(this->model()->index(0, 0, QModelIndex()));
}

bool ResultView::getSelectedState()
{
    return m_is_selected;
}

void ResultView::setSelectedState(bool state)
{
    m_is_selected = state;
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
 * @brief ResultView::onRowSelectedSlot 处理列表项选中事件
 * @param selected
 * @param deselected
 */
void ResultView::onRowSelectedSlot(const QModelIndex &index)
{
    //NEW_TODO
    if (index.isValid()) {
        m_is_selected = true;
        this->setCurrentIndex(index);
        Q_EMIT this->currentRowChanged(m_plugin_id, m_model->getInfo(index));
    }
}

void ResultView::onItemListChanged(const int &count)
{
    m_count = count;
    Q_EMIT this->listLengthChanged(count);
    QModelIndex index = this->currentIndex();
    m_model->refresh();
    this->setCurrentIndex(index);
}

void ResultView::setExpanded(const bool &is_expanded)
{
    QModelIndex index = this->currentIndex();
    m_model->setExpanded(is_expanded);
    this->setCurrentIndex(index);
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
//        plugin->openAction(action->text(), m_model->getKey(this->currentIndex()));
    } else {
        qWarning()<<"Get plugin failed!";
    }
}

void ResultView::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "result view press event source" << event->source();
    m_tmpCurrentIndex = this->currentIndex();
    m_tmpMousePressIndex = indexAt(event->pos());
    if (m_tmpMousePressIndex.isValid() and m_tmpCurrentIndex != m_tmpMousePressIndex) {
        Q_EMIT this->clicked(m_tmpMousePressIndex);
    }

    return QTreeView::mousePressEvent(event);
}

void ResultView::mouseReleaseEvent(QMouseEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if (index.isValid()) {
        Q_EMIT this->clicked(index);
    } else {
        Q_EMIT this->clicked(this->currentIndex());
    }
    return QTreeView::mouseReleaseEvent(event);
}

void ResultView::mouseMoveEvent(QMouseEvent *event)
{
   m_tmpCurrentIndex = this->currentIndex();
   m_tmpMousePressIndex = indexAt(event->pos());
   if (m_tmpMousePressIndex.isValid() and m_tmpCurrentIndex != m_tmpMousePressIndex and event->source() != Qt::MouseEventSynthesizedByQt) {
       Q_EMIT this->clicked(m_tmpMousePressIndex);
   }
    return QTreeView::mouseMoveEvent(event);
}

bool ResultView::viewportEvent(QEvent *event)
{
     if (event->type() == QEvent::TouchBegin) {
         qDebug() << "TouchBegin==============";
         QTouchEvent *e = dynamic_cast<QTouchEvent *>(event);
         QMouseEvent me(QEvent::MouseButtonPress,
                        e->touchPoints().at(0).pos(),
                        this->mapTo(this->window(),e->touchPoints().at(0).pos().toPoint()),
                        this->mapToGlobal(e->touchPoints().at(0).pos().toPoint()),
                        Qt::LeftButton,Qt::LeftButton,Qt::NoModifier,Qt::MouseEventSynthesizedByApplication);
         QApplication::sendEvent(parent(), &me);
         m_touchTimer->start();
         event->accept();
         return true;
     } else if (event->type() == QEvent::TouchEnd) {
         qDebug() << "touchend==============" << m_touchTimer->remainingTime();
         if (m_touchTimer->remainingTime() > 0.001) {
             QTouchEvent *e = dynamic_cast<QTouchEvent *>(event);
             QMouseEvent me(QEvent::MouseButtonPress,
                            e->touchPoints().at(0).pos(),
                            this->mapTo(this->window(),e->touchPoints().at(0).pos().toPoint()),
                            this->mapToGlobal(e->touchPoints().at(0).pos().toPoint()),
                            Qt::LeftButton,Qt::LeftButton,Qt::NoModifier,Qt::MouseEventSynthesizedByApplication);
             QApplication::sendEvent(this->viewport(),&me);

             QMouseEvent mer(QEvent::MouseButtonRelease,
                             e->touchPoints().at(0).pos(),
                             this->mapTo(this->window(),e->touchPoints().at(0).pos().toPoint()),
                             this->mapToGlobal(e->touchPoints().at(0).pos().toPoint()),
                             Qt::LeftButton,Qt::LeftButton,Qt::NoModifier,Qt::MouseEventSynthesizedByApplication);
             QApplication::sendEvent(this->viewport(),&mer);
         }
         return true;
     } else if (event->type() == QEvent::TouchUpdate) {
         qDebug() << "touchupdate==============";
         QTouchEvent *e = dynamic_cast<QTouchEvent *>(event);
         QMouseEvent me(QEvent::MouseMove,
                        e->touchPoints().at(0).pos(),
                        this->mapTo(this->window(),e->touchPoints().at(0).pos().toPoint()),
                        this->mapToGlobal(e->touchPoints().at(0).pos().toPoint()),
                        Qt::LeftButton,Qt::LeftButton,Qt::NoModifier,Qt::MouseEventSynthesizedByApplication);
         QApplication::sendEvent(parent(), &me);
         return true;
     }
    return QTreeView::viewportEvent(event);
}

void ResultView::initConnections()
{
    connect(this, &ResultView::startSearch, [ = ](const QString &keyword) {
        setExpanded(false);
        Q_EMIT this->lableReset();
        m_styleDelegate->setSearchKeyword(keyword);
        m_model->startSearch(keyword);
    });
    connect(this, &ResultView::stopSearch, m_model, &SearchResultModel::stopSearch);
    //connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ResultView::onRowSelectedSlot);
    connect(this, &ResultView::clicked, this, &ResultView::onRowSelectedSlot);
    connect(this, &ResultView::activated, this, &ResultView::onRowDoubleClickedSlot);
    connect(m_model, &SearchResultModel::itemListChanged, this, &ResultView::onItemListChanged);
}
