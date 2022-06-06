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
#include "search-page-section.h"
#include <QDebug>
#include <QScrollBar>
using namespace UkuiSearch;

#define RESULT_LAYOUT_MARGINS 0,0,0,0
#define RESULT_BACKGROUND_COLOR QColor(0, 0, 0, 0)
#define DETAIL_BACKGROUND_COLOR QColor(0, 0, 0, 0)
#define DETAIL_WIDGET_TRANSPARENT 0.04
#define DETAIL_WIDGET_BORDER_RADIUS 4
#define DETAIL_WIDGET_MARGINS 8,0,8,0
#define DETAIL_FRAME_MARGINS 8,0,0,0
#define DETAIL_ICON_HEIGHT 120
#define NAME_LABEL_WIDTH 280
#define ICON_SIZE QSize(120, 120)
#define LINE_STYLE "QFrame{background: rgba(0,0,0,0.2);}"
#define ACTION_NORMAL_COLOR QColor(55, 144, 250, 255)
#define ACTION_HOVER_COLOR QColor(64, 169, 251, 255)
#define ACTION_PRESS_COLOR QColor(41, 108, 217, 255)
#define TITLE_HEIGHT 30
#define FRAME_HEIGHT 516
#define DETAIL_FRAME_WIDTH 376

ResultArea::ResultArea(QWidget *parent) : QScrollArea(parent)
{
    qRegisterMetaType<SearchPluginIface::ResultInfo>("SearchPluginIface::ResultInfo");
    this->viewport()->setAttribute(Qt::WA_AcceptTouchEvents);
    initUi();
    initConnections();
}

void ResultArea::appendWidet(ResultWidget *widget)
{
    m_mainLyt->addWidget(widget);
    setupConnectionsForWidget(widget);
    widget->clearResult();
    m_widget_list.append(widget);
    int spacing_height = m_widget_list.length() > 1 ? m_mainLyt->spacing() : 0;
    m_widget->setFixedHeight(m_widget->height() + widget->height() + spacing_height);
}

/**
 * @brief ResultArea::setVisibleList 设置哪些插件可见，默认全部可见
 * @param list
 */
void ResultArea::setVisibleList(const QStringList &list)
{
    Q_FOREACH (auto widget, m_widget_list) {
        if (list.contains(widget->pluginId())) {
            widget->setEnabled(true);
        } else {
            widget->setEnabled(false);
        }
    }
}

void ResultArea::pressEnter()
{
    if (false == m_is_selected) {//未选中时默认选取bestlist第一项
        int resultNum = m_bestListWidget->getResultNum();
        if (0 == resultNum) {//无搜索结果时默认选中websearch
            for (ResultWidget * i : m_widget_list) {
                if (m_selectedPluginID == m_widget_list.back()->pluginId()) {
                    QModelIndex index = i->getModlIndex(0, 0);
                    i->setResultSelection(index);
                    m_selectedPluginID = i->pluginId();
                    m_is_selected = true;
                    break;
                }
            }
        } else {//选取bestlist第一项
            QModelIndex index = m_bestListWidget->getModlIndex(0, 0);
            m_bestListWidget->setResultSelection(index);
            m_selectedPluginID = m_bestListWidget->getWidgetName();
            m_is_selected = true;
        }
    } else {//选中状态时默认启动action首项
        //先判断详情页是否打开
        if (m_detail_open_state) {
            if (m_selectedPluginID == m_bestListWidget->getWidgetName()) {//最佳匹配
                m_bestListWidget->activateIndex();
            } else {
                for (ResultWidget * i : m_widget_list) {
                    if (m_selectedPluginID == i->pluginId()) {
                        i->activateIndex();
                        break;
                    }
                }
            }
        } else {//打开详情页
            m_detail_open_state = true;
            sendKeyPressSignal(m_selectedPluginID);
        }
    }
}

void ResultArea::pressDown()
{
    if (m_selectedPluginID == m_widget_list.back()->pluginId()) {//当前为web search，暂不处理
        return;
    } else if (m_selectedPluginID == m_bestListWidget->getWidgetName()) {
        QModelIndex index = m_bestListWidget->getCurrentSelection();
        int maxNum = m_bestListWidget->getExpandState() ?
                    m_bestListWidget->getResultNum() : (m_bestListWidget->getResultNum() < NUM_LIMIT_SHOWN_DEFAULT ?
                                                            m_bestListWidget->getResultNum() : NUM_LIMIT_SHOWN_DEFAULT);
        if (index.row() < maxNum - 1 and index.row() >= 0) {
            int row = index.row();
            QModelIndex setIndex = m_bestListWidget->getModlIndex(++row, 0);
            m_bestListWidget->setResultSelection(setIndex);
            sendKeyPressSignal(m_selectedPluginID);
        } else if (index.row() >= maxNum - 1 or index.row() < 0) {//跳转下一个widget
            m_bestListWidget->clearResultSelection();
            for (ResultWidget * plugin : m_widget_list) {
                if (plugin->getResultNum() != 0) {
                    QModelIndex resultIndex = plugin->getModlIndex(0, 0);
                    plugin->setResultSelection(resultIndex);
                    m_selectedPluginID = plugin->pluginId();
                    sendKeyPressSignal(m_selectedPluginID);
                    break;
                }
            }
        } else {
            qWarning() << "QModelIndex error ! row:" << index.row() << "maxNum:" << maxNum;
        }
    } else {
        for (ResultWidget * plugin : m_widget_list) {
            if (m_selectedPluginID == plugin->pluginId()) {
                QModelIndex index = plugin->getCurrentSelection();
                int maxNum = plugin->getExpandState() ?
                            plugin->getResultNum() : (plugin->getResultNum() < NUM_LIMIT_SHOWN_DEFAULT ?
                                                                    plugin->getResultNum() : NUM_LIMIT_SHOWN_DEFAULT);
                if (index.row() < maxNum - 1 and index.row() >= 0) {
                    int row = index.row();
                    QModelIndex setIndex = plugin->getModlIndex(++row, 0);
                    plugin->setResultSelection(setIndex);
                    sendKeyPressSignal(m_selectedPluginID);
                } else if (index.row() >= maxNum - 1 or index.row() < 0) {//跳转下一个widget
                    plugin->clearResultSelection();
                    int indexNum = m_widget_list.indexOf(plugin);
                    bool findNextWidget = false;
                    while (++indexNum < m_widget_list.size()) {
                        plugin = m_widget_list[indexNum];
                        if (plugin->getResultNum() != 0) {
                            QModelIndex resultIndex = plugin->getModlIndex(0, 0);
                            plugin->setResultSelection(resultIndex);
                            m_selectedPluginID = plugin->pluginId();
                            findNextWidget = true;
                            sendKeyPressSignal(m_selectedPluginID);
                            break;
                        }
                    }
                    if (findNextWidget){
                        break;
                    }
                } else {
                    qWarning() << "QModelIndex error ! row:" << index.row() << "maxNum:" << maxNum;
                }
            }
        }
    }
}

void ResultArea::pressUp()
{
    if (!m_is_selected) {
        return;
    }
    if (m_selectedPluginID == m_bestListWidget->getWidgetName()) {
        QModelIndex index = m_bestListWidget->getCurrentSelection();
        int maxNum = m_bestListWidget->getExpandState() ?
                    m_bestListWidget->getResultNum() : (m_bestListWidget->getResultNum() < NUM_LIMIT_SHOWN_DEFAULT ?
                                                            m_bestListWidget->getResultNum() : NUM_LIMIT_SHOWN_DEFAULT);
        if (index.row() > 0 and index.row() < maxNum) {
            int row = index.row();
            QModelIndex setIndex = m_bestListWidget->getModlIndex(--row, 0);
            m_bestListWidget->setResultSelection(setIndex);
            sendKeyPressSignal(m_selectedPluginID);
        } else if (index.row() == 0) {
            //已到最上层，暂不处理
        } else {
            QModelIndex setIndex = m_bestListWidget->getModlIndex(--maxNum, 0);
            m_bestListWidget->setResultSelection(setIndex);
            sendKeyPressSignal(m_selectedPluginID);
        }
    } else {
        for (ResultWidget * plugin : m_widget_list) {
            if (m_selectedPluginID == plugin->pluginId()) {
                int indexMaxNum = plugin->getExpandState() ?
                            plugin->getResultNum() : (plugin->getResultNum() < NUM_LIMIT_SHOWN_DEFAULT ?
                                                                    plugin->getResultNum() : NUM_LIMIT_SHOWN_DEFAULT);
                QModelIndex index = plugin->getCurrentSelection();
                if (index.row() > 0 and index.row() < indexMaxNum) {
                    int row = index.row();
                    QModelIndex setIndex = plugin->getModlIndex(--row, 0);
                    plugin->setResultSelection(setIndex);
                    sendKeyPressSignal(m_selectedPluginID);
                } else if (index.row() == 0) {//跳转下一个widget
                    plugin->clearResultSelection();
                    int indexNum = m_widget_list.indexOf(plugin);
                    bool findNextWidget = false;
                    while (--indexNum >= 0) {
                        plugin = m_widget_list[indexNum];
                        if (plugin->getResultNum() != 0) {
                            int maxNum = plugin->getExpandState() ?
                                        plugin->getResultNum() : (plugin->getResultNum() < NUM_LIMIT_SHOWN_DEFAULT ?
                                                                                plugin->getResultNum() : NUM_LIMIT_SHOWN_DEFAULT);
                            QModelIndex resultIndex = plugin->getModlIndex(maxNum - 1, 0);
                            plugin->setResultSelection(resultIndex);
                            m_selectedPluginID = plugin->pluginId();
                            findNextWidget = true;
                            sendKeyPressSignal(m_selectedPluginID);
                            break;
                        }
                    }
                    if (indexNum < 0) {//下一项是best list
                        int bestListNum = m_bestListWidget->getExpandState() ?
                                    m_bestListWidget->getResultNum() : (m_bestListWidget->getResultNum() < NUM_LIMIT_SHOWN_DEFAULT ?
                                                                            m_bestListWidget->getResultNum() : NUM_LIMIT_SHOWN_DEFAULT);
                        QModelIndex setIndex = m_bestListWidget->getModlIndex(--bestListNum, 0);
                        m_bestListWidget->setResultSelection(setIndex);
                        m_selectedPluginID = m_bestListWidget->getWidgetName();
                        m_is_selected = true;
                        sendKeyPressSignal(m_selectedPluginID);
                    }
                    if (findNextWidget){
                        break;
                    }
                } else {
                    QModelIndex setIndex = plugin->getModlIndex(indexMaxNum - 1, 0);
                    plugin->setResultSelection(setIndex);
                    sendKeyPressSignal(m_selectedPluginID);
                }
            }
        }
    }
}

bool ResultArea::getSelectedState()
{
    return m_is_selected;
}

void ResultArea::sendKeyPressSignal(QString &pluginID)
{
    int height(0);
    int resultHeight = m_bestListWidget->getResultHeight();
    if (pluginID == m_bestListWidget->getWidgetName()) {
        QModelIndex index = m_bestListWidget->getCurrentSelection();
        height = index.row() == 0 ? 0 : index.row() * resultHeight + TITLE_HEIGHT;
        height = (height - resultHeight) < 0 ? 0 : height - resultHeight;
        this->ensureVisible(0, height, 0, 0);
        if (m_detail_open_state) {
            Q_EMIT this->keyPressChanged(m_bestListWidget->getPluginInfo(index), m_bestListWidget->getIndexResultInfo(index));
        }
    } else {
        height += m_bestListWidget->height();
        for (ResultWidget *plugin : m_widget_list) {
            if (pluginID == plugin->pluginId()) {
                QModelIndex index = plugin->getCurrentSelection();
                height += index.row() == 0 ? 0 : index.row() * resultHeight + TITLE_HEIGHT;
                int moreHeight = index.row() == 0 ? (TITLE_HEIGHT + resultHeight * 2) : (resultHeight * 2);
                this->ensureVisible(0, height + moreHeight, 0, 0);
                height = (height - resultHeight) < 0 ? 0 : height - resultHeight;
                this->ensureVisible(0, height, 0, 0);
                if (m_detail_open_state) {
                    Q_EMIT this->keyPressChanged(m_selectedPluginID, plugin->getIndexResultInfo(index));
                }
                break;
            } else {
                height += plugin->height();
            }
        }
    }
}

void ResultArea::onWidgetSizeChanged()
{
    int whole_height = 0;
    Q_FOREACH (ResultWidget *widget, m_widget_list) {
        whole_height += widget->height();
    }
    whole_height += m_bestListWidget->height();

    int spacing_height = m_widget_list.length() > 1 ? m_mainLyt->spacing() : 0;
    m_widget->setFixedHeight(whole_height + spacing_height * (m_widget_list.length() - 1));
    Q_EMIT this->resizeHeight(whole_height + spacing_height * (m_widget_list.length() - 1));
}

void ResultArea::setSelectionInfo(QString &pluginID)
{
    m_detail_open_state = true;
    m_is_selected = true;
    m_selectedPluginID = pluginID;
    if (m_selectedPluginID != m_bestListWidget->getWidgetName()) {
        m_bestListWidget->clearResultSelection();
    }
}

void ResultArea::mousePressEvent(QMouseEvent *event)
{
//    qDebug() << "mouse pressed"  << event->source() << event->button();
//    m_pressPoint = event->pos();
    return QScrollArea::mousePressEvent(event);
}

void ResultArea::mouseMoveEvent(QMouseEvent *event)
{
//    if(m_pressPoint.isNull()) {
//        return QScrollArea::mouseMoveEvent(event);
//    }
//    int delta = ((event->pos().y() - m_pressPoint.y()) / this->widget()->height()) * verticalScrollBar()->maximum();
//    this->verticalScrollBar()->setValue(verticalScrollBar()->value() + delta);
//    event->accept();
    return QScrollArea::mouseMoveEvent(event);
}

void ResultArea::mouseReleaseEvent(QMouseEvent *event)
{
    return QScrollArea::mouseReleaseEvent(event);
}

bool ResultArea::viewportEvent(QEvent *event)
{
    if(event->type() == QEvent::TouchBegin) {
        QTouchEvent *e = dynamic_cast<QTouchEvent *>(event);
        if(e->touchPoints().size() == 1) {
            m_pressPoint = m_widget->mapFrom(this, e->touchPoints().at(0).pos().toPoint());
            event->accept();
            return true;
        }
    } else if (event->type() == QEvent::TouchUpdate) {
        QTouchEvent *e = dynamic_cast<QTouchEvent *>(event);
//        qDebug() << "touchpoint===========" << e->touchPoints().size();
        if(e->touchPoints().size() == 1) {
            int delta = m_pressPoint.y() - m_widget->mapFrom(this, e->touchPoints().at(0).pos().toPoint()).y();
//            qDebug() << "last pos:" << m_pressPoint.y();
//            qDebug() << "new pos:" << m_widget->mapFrom(this, e->touchPoints().at(0).pos().toPoint()).y();
//            qDebug() << "delta" << delta;
//            qDebug() << "height" << m_widget->height() << "--" << verticalScrollBar()->maximum();
            this->verticalScrollBar()->setValue(verticalScrollBar()->value() + delta);
            m_pressPoint = m_widget->mapFrom(this,e->touchPoints().at(0).pos().toPoint());
            return true;
        }
    }
    return QScrollArea::viewportEvent(event);
}

void ResultArea::initUi()
{
//    this->verticalScrollBar()->setProperty("drawScrollBarGroove", false);
    QPalette pal = palette();
    QPalette scroll_bar_pal = this->verticalScrollBar()->palette();
//    pal.setColor(QPalette::Base, RESULT_BACKGROUND_COLOR);
    pal.setColor(QPalette::Window, RESULT_BACKGROUND_COLOR);
    scroll_bar_pal.setColor(QPalette::Base, RESULT_BACKGROUND_COLOR);
    this->verticalScrollBar()->setPalette(scroll_bar_pal);
    this->verticalScrollBar()->setProperty("drawScrollBarGroove", false);
    this->setFrameShape(QFrame::Shape::NoFrame);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->setPalette(pal);
    this->setWidgetResizable(true);
    this->setFrameShape(QFrame::Shape::NoFrame);
    m_widget = new QWidget(this);
    this->setWidget(m_widget);
    m_mainLyt = new QVBoxLayout(m_widget);
    m_widget->setLayout(m_mainLyt);
    m_bestListWidget = new BestListWidget(this);
    m_bestListWidget->clearResult();
    m_mainLyt->addWidget(m_bestListWidget);

    m_mainLyt->setContentsMargins(RESULT_LAYOUT_MARGINS);
    this->widget()->setContentsMargins(0,0,0,0);
    m_mainLyt->setSpacing(0);

    m_titleLabel = new TitleLabel(this);
    m_titleLabel->hide();
}

void ResultArea::initConnections()
{
    connect(this, &ResultArea::startSearch, m_bestListWidget, &BestListWidget::startSearch);
    connect(this, &ResultArea::startSearch, this, [=] () {
        m_detail_open_state = false;
        m_is_selected = false;
        m_selectedPluginID = "";
    });
    connect(m_bestListWidget, &BestListWidget::sizeChanged, this, &ResultArea::onWidgetSizeChanged);
    connect(m_bestListWidget, &BestListWidget::sizeChanged, this, [=] () {
        QModelIndex index = m_bestListWidget->getModlIndex(0, 0);
        if (index.isValid()) {
            m_bestListWidget->setResultSelection(index);
            m_selectedPluginID = m_bestListWidget->getWidgetName();
            m_is_selected = true;

            for (ResultWidget * i : m_widget_list) {
                if (i->pluginId() == m_widget_list.back()->pluginId()) {
                    i->clearResultSelection();
                    break;
                }
            }
        }
    });

    connect(m_bestListWidget, &BestListWidget::currentRowChanged, this, &ResultArea::currentRowChanged);
    connect(m_bestListWidget, &BestListWidget::currentRowChanged, this, [=] () {
        m_detail_open_state = true;
        m_is_selected = true;
        m_selectedPluginID = m_bestListWidget->getWidgetName();
    });
    connect(this, &ResultArea::clearSelectedRow, m_bestListWidget, &BestListWidget::clearSelectedRow);
    connect(this, &ResultArea::resizeWidth, this, [=] (const int &size) {
        m_bestListWidget->setFixedWidth(size);
    });
    connect(m_bestListWidget, &BestListWidget::rowClicked, this, &ResultArea::rowClicked);
    connect(this->verticalScrollBar(), &QScrollBar::valueChanged, this, [=] (int value) {//判断显示和隐藏逻辑
        Q_FOREACH(auto widget, m_widget_list) {
            if (!widget->getExpandState()) {
                continue;
            }
            if (value < (widget->pos().ry() + TITLE_HEIGHT)
                    or value > (widget->pos().ry() + widget->height() - FRAME_HEIGHT + 2*TITLE_HEIGHT)) {//暂定下一项标题显示完全后悬浮标题隐藏
                if (!m_titleLabel->isHidden()) {
                    m_titleLabel->hide();
                    this->setViewportMargins(0,0,0,0);
                }
            } else {
                if (m_titleLabel->isHidden()) {
                    m_titleLabel->setText(widget->pluginName());
                    m_titleLabel->setShowMoreLableVisible();//防止多项展开后无法收回其他项
                    m_titleLabel->show();
                    this->setViewportMargins(0,TITLE_HEIGHT,0,0);
                }
                break;
            }
        }
    });
    connect(this->m_titleLabel, &TitleLabel::retractClicked, this, [=] () {
        Q_FOREACH(auto widget, m_widget_list) {
            if (widget->pluginName() == m_titleLabel->text()) {
                widget->reduceListSlot();
                widget->resetTitleLabel();
                if (!m_titleLabel->isHidden()) {
                    m_titleLabel->hide();
                    this->setViewportMargins(0,0,0,0);
                }
            }
        }
    });
    connect(this, &ResultArea::resizeWidth, this, [=] (int size) {
        m_titleLabel->setFixedWidth(size);
    });
}

void ResultArea::setupConnectionsForWidget(ResultWidget *widget)
{
    connect(this, &ResultArea::startSearch, widget, &ResultWidget::startSearch);
    connect(this, &ResultArea::startSearch, this, [=] () {
        if (!m_titleLabel->isHidden()) {
            m_titleLabel->hide();
            this->setViewportMargins(0,0,0,0);
        }
    });
    connect(this, &ResultArea::stopSearch, widget, &ResultWidget::stopSearch);
    connect(widget, &ResultWidget::sizeChanged, this, &ResultArea::onWidgetSizeChanged);
    connect(widget, &ResultWidget::sizeChanged, this, [=] () {
        if (widget->pluginId() == m_widget_list.back()->pluginId() and m_selectedPluginID != m_bestListWidget->getWidgetName()) {//每次搜索默认选中websearch，由bestlist取消
            QModelIndex index = widget->getModlIndex(0, 0);
            if (index.isValid()) {
                widget->setResultSelection(index);
                m_is_selected = true;
                m_selectedPluginID = widget->pluginId();
            }
        }
    });
    connect(widget, &ResultWidget::showMoreClicked, this, [=] () {//点击展开搜索结果后   显示悬浮窗
        if (widget->height() > FRAME_HEIGHT) {//当前widget高度大于搜索结果界面高度则显示悬浮窗
            this->verticalScrollBar()->setValue(widget->pos().ry() + TITLE_HEIGHT); //置顶当前类型搜索结果,不显示标题栏
            viewport()->stackUnder(m_titleLabel);
            m_titleLabel->setText(widget->pluginName());
            m_titleLabel->setFixedSize(widget->width(), TITLE_HEIGHT);
            m_titleLabel->setShowMoreLableVisible();
            if (m_titleLabel->isHidden()) {
                m_titleLabel->show();
                this->setViewportMargins(0,TITLE_HEIGHT,0,0);
            }
        } else {
            this->verticalScrollBar()->setValue(widget->pos().ry()); //置顶当前类型搜索结果，显示标题栏
        }
    });
    connect(widget, &ResultWidget::retractClicked, this, [=] () {//点击收起搜索结果后
        if (!m_titleLabel->isHidden()) {
            m_titleLabel->hide();
            this->setViewportMargins(0,0,0,0);
        }
    });
    connect(widget, &ResultWidget::sendBestListData, m_bestListWidget, &BestListWidget::sendBestListData);
}

DetailArea::DetailArea(QWidget *parent) : QScrollArea(parent)
{
    initUi();
    connect(this, &DetailArea::setWidgetInfo, m_detailWidget, &DetailWidget::updateDetailPage);
    connect(this, &DetailArea::setWidgetInfo, this, &DetailArea::show);
}

void DetailArea::initUi()
{
    QPalette pal = palette();
//    pal.setColor(QPalette::Base, DETAIL_BACKGROUND_COLOR);
    pal.setColor(QPalette::Window, DETAIL_BACKGROUND_COLOR);
    QPalette scroll_bar_pal = this->verticalScrollBar()->palette();
    scroll_bar_pal.setColor(QPalette::Base, RESULT_BACKGROUND_COLOR);
    this->verticalScrollBar()->setPalette(scroll_bar_pal);
    this->setPalette(pal);
    this->setFrameShape(QFrame::Shape::NoFrame);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->setWidgetResizable(true);
    this->setFixedSize(DETAIL_FRAME_WIDTH, FRAME_HEIGHT);
//    this->setStyleSheet("QScrollArea{border:2px solid red;}");
    this->setContentsMargins(0,0,0,0);
    m_detailWidget = new DetailWidget(this);
    this->setWidget(m_detailWidget);
    this->hide();
}

DetailWidget::DetailWidget(QWidget *parent) : QWidget(parent)
{
//    initUi();
//    clear();
    this->setFixedWidth(368);
    m_mainLyt = new QVBoxLayout(this);
    this->setLayout(m_mainLyt);
    m_mainLyt->setContentsMargins(DETAIL_WIDGET_MARGINS);
    m_mainLyt->setAlignment(Qt::AlignHCenter);
//    m_mainLyt->addStretch();
}

QString escapeHtml(const QString & str) {
    QString temp = str;
    temp.replace("<", "&lt;");
    temp.replace(">", "&gt;");
    return temp;
}

void DetailWidget::setWidgetInfo(const QString &plugin_name, const SearchPluginIface::ResultInfo &info)
{
//    clearLayout(m_descFrameLyt);
//    clearLayout(m_previewFrameLyt);
//    if(SearchPluginManager::getInstance()->getPlugin(plugin_name)->isPreviewEnable(info.actionKey,info.type)) {
//        m_iconLabel->hide();
//        m_previewFrameLyt->addWidget(SearchPluginManager::getInstance()->getPlugin(plugin_name)->previewPage(info.actionKey,info.type, m_previewFrame), 0 , Qt::AlignHCenter);
//        m_previewFrameLyt->setContentsMargins(0,0,0,0);
//        m_previewFrame->show();
//    } else {
//        m_previewFrame->hide();
//        m_iconLabel->setPixmap(info.icon.pixmap(info.icon.actualSize(ICON_SIZE)));
//        m_iconLabel->show();
//    }
//    QFontMetrics fontMetrics = m_nameLabel->fontMetrics();
//    QString name = fontMetrics.elidedText(info.name, Qt::ElideRight, NAME_LABEL_WIDTH - 8);
//    m_nameLabel->setText(QString("<h3 style=\"font-weight:normal;\">%1</h3>").arg(escapeHtml(name)));
//    m_nameLabel->setToolTip(info.name);
//    m_pluginLabel->setText(plugin_name);
//    m_nameFrame->show();
//    m_line_1->show();

//    if (info.description.length() > 0) {
//        //NEW_TODO 样式待优化
//        clearLayout(m_descFrameLyt);
//        Q_FOREACH (SearchPluginIface::DescriptionInfo desc, info.description) {
//            QLabel * descLabel = new QLabel(m_descFrame);
//            descLabel->setTextFormat(Qt::PlainText);
//            descLabel->setWordWrap(true);
//            QString show_desc = desc.key + "    " + desc.value;
//            descLabel->setText(show_desc);
//            m_descFrameLyt->addWidget(descLabel);
//        }
//        m_descFrame->show();
//        m_line_2->show();
//    }
//    clearLayout(m_actionFrameLyt);
//    Q_FOREACH (SearchPluginIface::Actioninfo actioninfo, SearchPluginManager::getInstance()->getPlugin(plugin_name)->getActioninfo(info.type)) {
//        ActionLabel * actionLabel = new ActionLabel(actioninfo.displayName, info.actionKey, actioninfo.actionkey, plugin_name, info.type, m_actionFrame);
//        m_actionFrameLyt->addWidget(actionLabel);
//    }
//    m_actionFrame->show();
}

void DetailWidget::updateDetailPage(const QString &plugin_name, const SearchPluginIface::ResultInfo &info)
{
    if(m_detailPage) {
        if(m_currentPluginId == plugin_name) {
            SearchPluginManager::getInstance()->getPlugin(plugin_name)->detailPage(info);
        } else {
            m_mainLyt->removeWidget(m_detailPage);
            m_detailPage->hide();
            m_detailPage = SearchPluginManager::getInstance()->getPlugin(plugin_name)->detailPage(info);
            m_detailPage->setParent(this);
            m_mainLyt->addWidget(m_detailPage);
            m_detailPage->show();
//            m_mainLyt->insertWidget(0, m_detailPage, 0);
        }
    } else {
        m_detailPage = SearchPluginManager::getInstance()->getPlugin(plugin_name)->detailPage(info);
        m_detailPage->setParent(this);
        m_mainLyt->addWidget(m_detailPage);
//        m_mainLyt->insertWidget(0, m_detailPage, 0);
    }
    m_currentPluginId = plugin_name;
}

void DetailWidget::clear()
{
//    m_iconLabel->hide();
//    m_nameFrame->hide();
//    m_line_1->hide();
//    m_descFrame->hide();
//    m_line_2->hide();
//    m_actionFrame->hide();
}

void DetailWidget::initUi()
{
//    this->setFixedSize(368, 516);
//    m_mainLyt = new QVBoxLayout(this);
//    this->setLayout(m_mainLyt);
//    m_mainLyt->setContentsMargins(DETAIL_WIDGET_MARGINS);
//    m_mainLyt->setAlignment(Qt::AlignHCenter);

//    m_iconLabel = new QLabel(this);
//    m_iconLabel->setFixedHeight(DETAIL_ICON_HEIGHT);
//    m_iconLabel->setAlignment(Qt::AlignCenter);
//    m_previewFrame = new QFrame(this);
//    m_previewFrameLyt = new QHBoxLayout(m_previewFrame);

//    m_nameFrame = new QFrame(this);
//    m_nameFrameLyt = new QHBoxLayout(m_nameFrame);
//    m_nameFrame->setLayout(m_nameFrameLyt);
//    m_nameFrameLyt->setContentsMargins(DETAIL_FRAME_MARGINS);
//    m_nameLabel = new QLabel(m_nameFrame);
//    m_nameLabel->setMaximumWidth(NAME_LABEL_WIDTH);
//    m_pluginLabel = new QLabel(m_nameFrame);
//    m_pluginLabel->setEnabled(false);
//    m_nameFrameLyt->addWidget(m_nameLabel);
//    m_nameFrameLyt->addStretch();
//    m_nameFrameLyt->addWidget(m_pluginLabel);

//    m_line_1 = new QFrame(this);
//    m_line_1->setFixedHeight(1);
//    m_line_1->setLineWidth(0);
//    m_line_1->setStyleSheet(LINE_STYLE);
//    m_line_2 = new QFrame(this);
//    m_line_2->setFixedHeight(1);
//    m_line_2->setLineWidth(0);
//    m_line_2->setStyleSheet(LINE_STYLE);

//    m_descFrame = new QFrame(this);
//    m_descFrameLyt = new QVBoxLayout(m_descFrame);
//    m_descFrame->setLayout(m_descFrameLyt);
//    m_descFrameLyt->setContentsMargins(DETAIL_FRAME_MARGINS);

//    m_actionFrame = new QFrame(this);
//    m_actionFrameLyt = new QVBoxLayout(m_actionFrame);
//    m_actionFrame->setLayout(m_actionFrameLyt);
//    m_actionFrameLyt->setContentsMargins(DETAIL_FRAME_MARGINS);

//    m_mainLyt->addWidget(m_iconLabel);
//    m_mainLyt->addWidget(m_previewFrame, 0, Qt::AlignHCenter);
//    m_mainLyt->addWidget(m_nameFrame);
//    m_mainLyt->addWidget(m_line_1);
//    m_mainLyt->addWidget(m_descFrame);
//    m_mainLyt->addWidget(m_line_2);
//    m_mainLyt->addWidget(m_actionFrame);
//    m_mainLyt->addStretch();
}

void DetailWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QRect rect = this->rect().adjusted(0, 0, 0, 0);
    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    p.setBrush(opt.palette.color(QPalette::Text));
    p.setOpacity(DETAIL_WIDGET_TRANSPARENT);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rect, DETAIL_WIDGET_BORDER_RADIUS, DETAIL_WIDGET_BORDER_RADIUS);
    return QWidget::paintEvent(event);
}

void DetailWidget::clearLayout(QLayout *layout)
{
    if(!layout) return;
    QLayoutItem * child;
    while((child = layout->takeAt(0)) != 0) {
        if(child->widget()) {
            child->widget()->setParent(NULL);
        }
        delete child;
    }
    child = NULL;
}

//ActionLabel::ActionLabel(const QString &action, const QString &key, const int &ActionKey, const QString &pluginId, const int type, QWidget *parent) : QLabel(parent)
//{
//    m_action = action;
//    m_key = key;
//    m_actionKey = ActionKey;
//    m_type = type;
//    m_pluginId = pluginId;
//    this->initUi();
//    this->installEventFilter(this);
//}

//void ActionLabel::initUi()
//{
//    this->setText(m_action);
//    QPalette pal = palette();
//    pal.setColor(QPalette::WindowText, ACTION_NORMAL_COLOR);
//    pal.setColor(QPalette::Light, ACTION_HOVER_COLOR);
//    pal.setColor(QPalette::Dark, ACTION_PRESS_COLOR);
//    this->setPalette(pal);
//    this->setForegroundRole(QPalette::WindowText);
//    this->setCursor(QCursor(Qt::PointingHandCursor));
//}

//bool ActionLabel::eventFilter(QObject *watched, QEvent *event)
//{
//    if (watched == this) {
//        if(event->type() == QEvent::MouseButtonPress) {
//            this->setForegroundRole(QPalette::Dark);
//            return true;
//        } else if(event->type() == QEvent::MouseButtonRelease) {
//            SearchPluginIface *plugin = SearchPluginManager::getInstance()->getPlugin(m_pluginId);
//            if (plugin)
//                plugin->openAction(m_actionKey, m_key, m_type);
//            else
//                qWarning()<<"Get plugin failed!";
//            this->setForegroundRole(QPalette::Light);
//            return true;
//        } else if(event->type() == QEvent::Enter) {
//            this->setForegroundRole(QPalette::Light);
//            return true;
//        } else if(event->type() == QEvent::Leave) {
//            this->setForegroundRole(QPalette::WindowText);
//            return true;
//        }
//    }
//}
