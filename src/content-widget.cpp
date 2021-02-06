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
#include "content-widget.h"
#include <QDebug>
#include <QLabel>
#include <QTimer>
#include "config-file.h"

ContentWidget::ContentWidget(QWidget * parent):QStackedWidget(parent)
{
    initUI();
    initListView();
//    m_quicklyOpenList<<"/usr/share/applications/peony.desktop"<<"/usr/share/applications/ukui-control-center.desktop"<<"Background/背景/更改壁纸";
    m_quicklyOpenList<<"/usr/share/applications/peony.desktop"<<"/usr/share/applications/ukui-control-center.desktop"<<"/usr/share/applications/ksc-defender.desktop";
}

ContentWidget::~ContentWidget()
{
    if (m_homePage) {
        delete m_homePage;
        m_homePage = nullptr;
    }
    if (m_resultPage) {
        delete m_resultPage;
        m_resultPage = nullptr;
    }
}

/**
 * @brief initUI 初始化homepage和resultpage
 */
void ContentWidget::initUI() {
    m_homePage = new QWidget;
    m_homePageLyt = new QVBoxLayout(m_homePage);
    m_homePageLyt->setSpacing(0);
    m_homePage->setLayout(m_homePageLyt);

    m_resultPage = new QWidget;
    m_resultPageLyt = new QHBoxLayout(m_resultPage);
    m_resultPageLyt->setSpacing(0);
    m_resultPageLyt->setContentsMargins(0, 0, 0, 0);
    m_resultListArea = new QScrollArea(m_resultPage);
    m_resultListArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_resultListArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_resultDetailArea = new QScrollArea(m_resultPage);
    m_resultDetailArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_resultDetailArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_resultListArea->setFixedWidth(244);
    m_resultPageLyt->addWidget(m_resultListArea);
    m_resultPageLyt->addWidget(m_resultDetailArea);
    m_resultPage->setLayout(m_resultPageLyt);

    m_resultList = new QWidget(m_resultDetailArea);
    m_resultDetail = new QWidget(m_resultDetailArea);
    m_listLyt = new QVBoxLayout(m_resultList);
    m_detailLyt = new QVBoxLayout(m_resultDetail);
    m_resultList->setFixedWidth(240);
    m_resultList->setFixedHeight(0);
    m_resultList->setStyleSheet("QWidget{background:transparent;}");
    m_listLyt->setContentsMargins(0, 0, 15, 0);
    m_listLyt->setSpacing(0);
    m_resultListArea->setWidget(m_resultList);
    m_resultListArea->setWidgetResizable(true);
    m_detailView = new SearchDetailView(m_resultDetailArea);
    connect(m_detailView, &SearchDetailView::configFileChanged, this, [ = ]() {
        clearLayout(m_homePageLyt);
        initHomePage();
    });
    m_resultDetailArea->setWidget(m_detailView);
    m_resultDetailArea->setWidgetResizable(true);
    m_resultListArea->setStyleSheet("QScrollArea{background: transparent;}");
    m_resultDetailArea->setStyleSheet("QScrollArea{background: transparent; border-radius: 4px;}");
    this->addWidget(m_homePage);
    this->addWidget(m_resultPage);

    setPage(SearchItem::SearchType::Best);//初始化按“最佳”加载
}

/**
 * @brief ContentWidget::initListView 初始化所有搜索结果列表
 */
void ContentWidget::initListView()
{
    m_fileListView = new SearchListView(m_resultList, QStringList(), SearchItem::SearchType::Files);
    m_dirListView = new SearchListView(m_resultList, QStringList(), SearchItem::SearchType::Dirs);
    m_contentListView = new SearchListView(m_resultList, QStringList(), SearchItem::SearchType::Contents);
    m_settingListView = new SearchListView(m_resultList, QStringList(), SearchItem::SearchType::Settings);
    m_appListView = new SearchListView(m_resultList, QStringList(), SearchItem::SearchType::Apps);
    m_bestListView = new SearchListView(m_resultList, QStringList(), SearchItem::SearchType::Best);
    setupConnect(m_fileListView);
    setupConnect(m_dirListView);
    setupConnect(m_contentListView);
    setupConnect(m_settingListView);
    setupConnect(m_appListView);
    setupConnect(m_bestListView);

    m_fileTitleLabel = new TitleLabel(m_resultList);
    m_fileTitleLabel->setText(getTitleName(SearchItem::SearchType::Files));
    m_dirTitleLabel = new TitleLabel(m_resultList);
    m_dirTitleLabel->setText(getTitleName(SearchItem::SearchType::Dirs));
    m_contentTitleLabel = new TitleLabel(m_resultList);
    m_contentTitleLabel->setText(getTitleName(SearchItem::SearchType::Contents));
    m_appTitleLabel = new TitleLabel(m_resultList);
    m_appTitleLabel->setText(getTitleName(SearchItem::SearchType::Apps));
    m_settingTitleLabel = new TitleLabel(m_resultList);
    m_settingTitleLabel->setText(getTitleName(SearchItem::SearchType::Settings));
    m_bestTitleLabel = new TitleLabel(m_resultList);
    m_bestTitleLabel->setText(getTitleName(SearchItem::SearchType::Best));

    m_appShowMoreLabel = new ShowMoreLabel(m_resultList);
    m_settingShowMoreLabel = new ShowMoreLabel(m_resultList);
    m_dirShowMoreLabel = new ShowMoreLabel(m_resultList);
    m_fileShowMoreLabel = new ShowMoreLabel(m_resultList);
    m_contentShowMoreLabel = new ShowMoreLabel(m_resultList);

    m_listLyt->addWidget(m_bestTitleLabel);
    m_listLyt->addWidget(m_bestListView);
    m_listLyt->addWidget(m_appTitleLabel);
    m_listLyt->addWidget(m_appListView);
    m_listLyt->addWidget(m_appShowMoreLabel);
    m_listLyt->addWidget(m_settingTitleLabel);
    m_listLyt->addWidget(m_settingListView);
    m_listLyt->addWidget(m_settingShowMoreLabel);
    m_listLyt->addWidget(m_dirTitleLabel);
    m_listLyt->addWidget(m_dirListView);
    m_listLyt->addWidget(m_dirShowMoreLabel);
    m_listLyt->addWidget(m_fileTitleLabel);
    m_listLyt->addWidget(m_fileListView);
    m_listLyt->addWidget(m_fileShowMoreLabel);
    m_listLyt->addWidget(m_contentTitleLabel);
    m_listLyt->addWidget(m_contentListView);
    m_listLyt->addWidget(m_contentShowMoreLabel);

    this->hideListView();
    m_resultList->setFixedHeight(0);

    connect(m_appShowMoreLabel, &ShowMoreLabel::showMoreClicked, this, [ = ]() {
        m_appListView->setList(m_appList);
        m_appShowMoreLabel->stopLoading();
        this->resetListHeight();
    });
    connect(m_appShowMoreLabel, &ShowMoreLabel::retractClicked, this, [ = ]() {
        m_appListView->setList(m_appList.mid(0, 5));
        m_appShowMoreLabel->stopLoading();
        this->resetListHeight();
    });
    connect(m_settingShowMoreLabel, &ShowMoreLabel::showMoreClicked, this, [ = ]() {
        m_settingListView->setList(m_settingList);
        m_settingShowMoreLabel->stopLoading();
        this->resetListHeight();
    });
    connect(m_settingShowMoreLabel, &ShowMoreLabel::retractClicked, this, [ = ]() {
        m_settingListView->setList(m_settingList.mid(0, 5));
        m_settingShowMoreLabel->stopLoading();
        this->resetListHeight();
    });
    connect(m_dirShowMoreLabel, &ShowMoreLabel::showMoreClicked, this, [ = ]() {
        m_dirListView->setList(m_dirList);
        m_dirShowMoreLabel->stopLoading();
        this->resetListHeight();
    });
    connect(m_dirShowMoreLabel, &ShowMoreLabel::retractClicked, this, [ = ]() {
        m_dirListView->setList(m_dirList.mid(0, 5));
        m_dirShowMoreLabel->stopLoading();
        this->resetListHeight();
    });
    connect(m_fileShowMoreLabel, &ShowMoreLabel::showMoreClicked, this, [ = ]() {
        m_fileListView->setList(m_fileList);
        m_fileShowMoreLabel->stopLoading();
        this->resetListHeight();
    });
    connect(m_fileShowMoreLabel, &ShowMoreLabel::retractClicked, this, [ = ]() {
        m_fileListView->setList(m_fileList.mid(0, 5));
        m_fileShowMoreLabel->stopLoading();
        this->resetListHeight();
    });
    connect(m_contentShowMoreLabel, &ShowMoreLabel::showMoreClicked, this, [ = ]() {
        m_contentListView->setList(m_contentList);
        m_contentShowMoreLabel->stopLoading();
        this->resetListHeight();
    });
    connect(m_contentShowMoreLabel, &ShowMoreLabel::retractClicked, this, [ = ]() {
        m_contentListView->setList(m_contentList.mid(0, 5));
        m_contentShowMoreLabel->stopLoading();
        this->resetListHeight();
    });
}

/**
 * @brief ContentWidget::hideListView 隐藏所有列表
 */
void ContentWidget::hideListView()
{
    m_bestTitleLabel->hide();
    m_bestListView->hide();
    m_appTitleLabel->hide();
    m_appListView->hide();
    m_appShowMoreLabel->hide();
    m_settingTitleLabel->hide();
    m_settingListView->hide();
    m_settingShowMoreLabel->hide();
    m_dirTitleLabel->hide();
    m_dirListView->hide();
    m_dirShowMoreLabel->hide();
    m_fileTitleLabel->hide();
    m_fileListView->hide();
    m_fileShowMoreLabel->hide();
    m_contentTitleLabel->hide();
    m_contentListView->hide();
    m_contentShowMoreLabel->hide();
}

/**
 * @brief ContentWidget::setupConnect 初始化各个treeview的信号与槽
 * @param listview
 */
void ContentWidget::setupConnect(SearchListView * listview) {
    connect(listview, &SearchListView::currentRowChanged, this, [ = ](const int& type, const QString& path) {
        if(type == SearchItem::SearchType::Contents && !m_contentDetailList.isEmpty()) {
            m_detailView->isContent = true;
            m_detailView->setContent(m_contentDetailList.at(listview->currentIndex().row()), m_keyword);
        } else if (type == SearchItem::SearchType::Best && !m_bestContent.isEmpty() && listview->currentIndex().row() == listview->getLength() - 1) {
            m_detailView->setContent(m_bestContent, m_keyword);
            m_detailView->isContent = true;
            m_detailView->setupWidget(type == SearchItem::SearchType::Contents, path);
            listview->is_current_list = true;
            Q_EMIT this->currentItemChanged();
            listview->is_current_list = false;
            return;
        } else {
            m_detailView->isContent = false;
        }
        m_detailView->setupWidget(type, path);
//        m_detailView->setWebWidget(this->m_keyword);
        listview->is_current_list = true;
        Q_EMIT this->currentItemChanged();
        listview->is_current_list = false;
    });
    connect(this, &ContentWidget::currentItemChanged, listview, [ = ]() {
        if (! listview->is_current_list) {
            listview->blockSignals(true);
            listview->clearSelection();
            listview->blockSignals(false);
        }
    });
}

/**
 * @brief ContentWidget::resetHeight 重新计算列表高度
 */
void ContentWidget::resetListHeight()
{
    int height = 0;
    if (! m_bestListView->isHidden) {
        height += m_bestTitleLabel->height();
        height += m_bestListView->height();
    }
    if (! m_appListView->isHidden) {
        height += m_appTitleLabel->height();
        height += m_appListView->height();
        if (m_appShowMoreLabel->isVisible()) {
            height += m_appShowMoreLabel->height();
        }
    }
    if (! m_settingListView->isHidden) {
        height += m_settingTitleLabel->height();
        height += m_settingListView->height();
        if (m_settingShowMoreLabel->isVisible()) {
            height += m_settingShowMoreLabel->height();
        }
    }
    if (! m_fileListView->isHidden) {
        height += m_fileTitleLabel->height();
        height += m_fileListView->height();
        if (m_fileShowMoreLabel->isVisible()) {
            height += m_fileShowMoreLabel->height();
        }
    }
    if (! m_dirListView->isHidden) {
        height += m_dirTitleLabel->height();
        height += m_dirListView->height();
        if (m_dirShowMoreLabel->isVisible()) {
            height += m_dirShowMoreLabel->height();
        }
    }
    if (! m_contentListView->isHidden) {
        height += m_contentTitleLabel->height();
        height += m_contentListView->height();
        if (m_contentShowMoreLabel->isVisible()) {
            height += m_contentShowMoreLabel->height();
        }
    }
    m_resultList->setFixedHeight(height);
}

/**
 * @brief ContentWidget::initHomePage 向homepage填充内容
 * @param lists 三个列表：常用，最近，快捷
 */
void ContentWidget::initHomePage() {
    QVector<QStringList> lists;
    QMap<QString, QStringList> map = ConfigFile::readConfig();
    QStringList commonlyList;
    commonlyList = map.value("Commonly");
    QStringList recentlyList;
    recentlyList = map.value("Recently");
    lists.append(commonlyList);
    lists.append(recentlyList);
    lists.append(m_quicklyOpenList);

    for (int i = 0; i < lists.count(); i++) {
        if (lists.at(i).isEmpty())
            continue;
        QWidget * listWidget = new QWidget(m_homePage);
        QVBoxLayout * itemWidgetLyt = new QVBoxLayout(listWidget);
        QLabel * titleLabel = new QLabel(listWidget);
        QWidget * itemWidget = new QWidget(listWidget);
        if (i == 1) {
            if (lists.at(i).length() <= 2) itemWidget->setFixedHeight(48);
            else itemWidget->setFixedHeight(112);
            titleLabel->setText(tr("Recently Opened"));
            QGridLayout * layout = new QGridLayout(itemWidget);
            layout->setSpacing(8);
            layout->setContentsMargins(0, 0, 0, 0);
            itemWidget->setLayout(layout);
            for (int j = 0; j < lists.at(i).count(); j++) {
                HomePageItem * item = new HomePageItem(itemWidget, i, lists.at(i).at(j));
                item->setFixedSize(265, 48);
                layout->addWidget(item, j / 2, j % 2);
            }
            if (lists.at(i).length() == 1) {
                QWidget * emptyItem = new QWidget(itemWidget);
                emptyItem->setFixedSize(265, 48); //占位用widget,只有一项时在右方补全
                layout->addWidget(emptyItem, 1, 2);
            }
        } else {
            itemWidget->setFixedHeight(136);
            QHBoxLayout * layout = new QHBoxLayout(itemWidget);
            layout->setSpacing(8);
            layout->setContentsMargins(0, 0, 0, 0);
            itemWidget->setLayout(layout);
            int shownItem = lists.at(i).length();
            Q_FOREACH(QString path, lists.at(i)){
                if (i && QString::compare(FileUtils::getAppName(path),"Unknown App") == 0) {
                    shownItem --;
                    continue;
                }
                HomePageItem * item = new HomePageItem(itemWidget, i, path);
                item->setFixedSize(100, 100);
                layout->addWidget(item);
            }
            for (int j = 0; j < 5 - shownItem; j++) {
                QWidget * emptyItem = new QWidget(itemWidget);
                emptyItem->setFixedSize(100, 100); //占位用widget,少于5项会补全后方占位
                layout->addWidget(emptyItem);
            }
            if (i && shownItem) titleLabel->setText(tr("Open Quickly"));
            else titleLabel->setText(tr("Commonly Used"));
        }
        itemWidgetLyt->setSpacing(6);
        titleLabel->setFixedHeight(24);
        itemWidgetLyt->addWidget(titleLabel);
        itemWidgetLyt->addWidget(itemWidget);
        m_homePageLyt->addWidget(listWidget);
    }
    m_homePageLyt->addStretch();
}

/**
 * @brief setPageType 预留的接口，为指定类别搜索调整界面内容
 * @param type
 */
void ContentWidget::setPage(const int& type) {
    m_currentType = type;
}

/**
 * @brief ContentWidget::currentType 返回当前内容页（home或searchresult）
 * @return
 */
int ContentWidget::currentPage() {
    return m_currentType;
}

/**
 * @brief ContentWidget::refreshSearchList 刷新/构建搜索结果列表
 * @param lists 获取到的应用与设置结果列表，list.at(0)是应用， list.at(1)是设置
 */
void ContentWidget::refreshSearchList(const QVector<QStringList>& lists) {
    this->hideListView();
    if (m_fileListView) {
        m_fileListView->hide();
        m_fileTitleLabel->hide();
        m_fileShowMoreLabel->hide();
        m_fileListView->isHidden = true;
        m_fileListView->clear();
    }
    if (m_dirListView) {
        m_dirListView->hide();
        m_dirTitleLabel->hide();
        m_dirShowMoreLabel->hide();
        m_dirListView->isHidden = true;
        m_dirListView->clear();
    }
    if (m_contentListView) {
        m_contentListView->hide();
        m_contentTitleLabel->hide();
        m_contentShowMoreLabel->hide();
        m_contentListView->isHidden = true;
        m_contentListView->clear();
    }
    if (m_appListView) {
        m_appListView->hide();
        m_appTitleLabel->hide();
        m_appShowMoreLabel->hide();
        m_appListView->isHidden = true;
        m_appListView->clear();
    }
    if (m_settingListView) {
        m_settingListView->hide();
        m_settingTitleLabel->hide();
        m_settingShowMoreLabel->hide();
        m_settingListView->isHidden = true;
        m_settingListView->clear();
    }
    if (m_bestListView) {
        m_bestListView->hide();
        m_bestTitleLabel->hide();
        m_bestListView->isHidden = true;
        m_bestListView->clear();
    }
    m_resultList->setFixedHeight(0);
    m_detailView->clearLayout();
    m_contentDetailList.clear();
    m_bestContent.clear();

    m_appShowMoreLabel->resetLabel();
    m_settingShowMoreLabel->resetLabel();
    m_dirShowMoreLabel->resetLabel();
    m_fileShowMoreLabel->resetLabel();
    m_contentShowMoreLabel->resetLabel();

    if (! m_appList.isEmpty())
        m_appList.clear();
    if (! m_settingList.isEmpty())
        m_settingList.clear();
    if (! m_dirList.isEmpty())
        m_dirList.clear();
    if (! m_fileList.isEmpty())
        m_fileList.clear();
    if (! m_contentList.isEmpty())
        m_contentList.clear();

    if (!lists.at(0).isEmpty()) {
        m_appList = lists.at(0);
        qDebug()<<"Append a best item into list: "<<lists.at(0).at(0);
        appendSearchItem(SearchItem::SearchType::Best, lists.at(0).at(0));
        m_appListView->show();
        m_appTitleLabel->show();
        m_appListView->isHidden = false;
        if (m_appList.length() <= 5) {
            m_appListView->setList(m_appList);
        } else {
            m_appShowMoreLabel->show();
            m_appListView->setList(m_appList.mid(0, 5));
        }
    }
    if (!lists.at(1).isEmpty()) {
        m_settingList = lists.at(1);
        qDebug()<<"Append a best item into list: "<<lists.at(1).at(0);
        appendSearchItem(SearchItem::SearchType::Best, lists.at(1).at(0));
        m_settingListView->show();
        m_settingTitleLabel->show();
        m_settingListView->isHidden = false;
        if (m_settingList.length() <= 5) {
            m_settingListView->setList(m_settingList);
        } else {
            m_settingShowMoreLabel->show();
            m_settingListView->setList(m_settingList.mid(0, 5));
        }
    }
    this->resetListHeight();
}

/**
 * @brief ContentWidget::appendSearchItem 向列表添加一项搜索结果
 * @param type 类型
 * @param path 路径
 * @param contents 文件内容
 */
void ContentWidget::appendSearchItem(const int& type, const QString& path, QStringList contents) {
    switch (type) {
        case SearchItem::SearchType::Best: {
            if (m_bestListView->isHidden) {
                m_bestListView->show();
                m_bestTitleLabel->show();
                m_bestListView->isHidden = false;
            }
            m_bestListView->appendItem(path);
            if (m_detailView->isEmpty()) {
                m_bestListView->setCurrentIndex(m_bestListView->model()->index(0, 0, QModelIndex()));
            }
            break;
        }
//        case SearchItem::SearchType::Apps: {
//            if (m_appListView->isHidden) {
//                m_appListView->show();
//                m_appTitleLabel->show();
//                m_appListView->isHidden = false;
//                if (!m_detailView->isEmpty() && m_detailView->getType() > type) {
//                    m_appListView->setCurrentIndex(m_appListView->model()->index(0, 0, QModelIndex()));
//                }
//            }
//            m_appListView->appendItem(path);
//            currentList = m_appListView;

//            this->resetListHeight();
//            break;
//        }
//        case SearchItem::SearchType::Settings: {
//            if (m_settingListView->isHidden) {
//                m_settingListView->show();
//                m_settingTitleLabel->show();
//                m_settingListView->isHidden = false;
//                if (!m_detailView->isEmpty() && m_detailView->getType() > type) {
//                    m_settingListView->setCurrentIndex(m_settingListView->model()->index(0, 0, QModelIndex()));
//                }
//            }
//            m_settingListView->appendItem(path);
//            currentList = m_settingListView;
//            break;
//        }
        case SearchItem::SearchType::Files: {
            if (m_fileListView->isHidden) {
                m_fileListView->show();
                m_fileTitleLabel->show();
                m_fileListView->isHidden = false;
                appendSearchItem(SearchItem::SearchType::Best, path);
            }
            if (m_fileListView->getLength() < 5) {
                m_fileListView->appendItem(path);
            } else if (m_fileListView->getLength() == 5) {
                m_fileShowMoreLabel->show();
            }
            m_fileList.append(path);
            break;;
        }
        case SearchItem::SearchType::Dirs: {
            if (m_dirListView->isHidden) {
                m_dirListView->show();
                m_dirTitleLabel->show();
                m_dirListView->isHidden = false;
                appendSearchItem(SearchItem::SearchType::Best, path);
            }
            if (m_dirListView->getLength() < 5) {
                m_dirListView->appendItem(path);
            } else if (m_dirListView->getLength() == 5) {
                m_dirShowMoreLabel->show();
            }
            m_dirList.append(path);
            break;
        }
        case SearchItem::SearchType::Contents: {
            if (m_contentListView->isHidden) {
                m_contentListView->show();
                m_contentTitleLabel->show();
                m_contentListView->isHidden = false;
                for (int i = 0; i < contents.length(); i ++) {
                    m_bestContent.append(contents.at(i));
                    if (i != contents.length() - 1) {
                        m_bestContent.append("\n");
                    }
                }
                appendSearchItem(SearchItem::SearchType::Best, path);
            }
            if (m_contentListView->getLength() < 5) {
                m_contentListView->appendItem(path);
            } else if (m_contentListView->getLength() == 5) {
                m_contentShowMoreLabel->show();
            }
            m_contentList.append(path);
            QString temp;
            for (int i = 0; i < contents.length(); i ++) {
                temp.append(contents.at(i));
                if (i != contents.length() - 1) {
                    temp.append("\n");
                }
            }
            m_contentDetailList.append(temp);
            break;
        }
        default:
            break;
    }
    this->resetListHeight();
    return;
}

/**
 * @brief ContentWidget::getTitleName 获取表头
 * @param type 搜索类型
 * @return
 */
QString ContentWidget::getTitleName(const int& type) {
    switch (type) {
        case SearchItem::SearchType::Apps :
            return tr("Apps");
        case SearchItem::SearchType::Settings :
            return tr("Settings");
        case SearchItem::SearchType::Files :
            return tr("Files");
        case SearchItem::SearchType::Dirs :
            return tr("Dirs");
        case SearchItem::SearchType::Contents :
            return tr("File Contents");
        case SearchItem::SearchType::Best :
            return tr("Best Matches");
        default :
            return tr("Unknown");
    }
}

/**
 * @brief ContentWidget::clearLayout 清空某个布局
 * @param layout 需要清空的布局
 */
void ContentWidget::clearLayout(QLayout * layout) {
    if (! layout) return;
    QLayoutItem * child;
    while ((child = layout->takeAt(0)) != 0) {
        if(child->widget())
        {
            child->widget()->setParent(NULL); //防止删除后窗口看上去没有消失
        }
        delete child;
    }
    child = NULL;
}


/**
 * @brief ContentWidget::setContentList 文本内容搜索结果列表（包含所有文本段）
 * @param list
 */
void ContentWidget::setContentList(const QStringList& list) {
    m_contentDetailList.clear();
    m_contentDetailList = list;
}

/**
 * @brief ContentWidget::setKeyword 设置关键字
 * @param keyword
 */
void ContentWidget::setKeyword(QString keyword)
{
    m_keyword = keyword;
    m_fileListView->setKeyword(keyword);
    m_dirListView->setKeyword(keyword);
    m_contentListView->setKeyword(keyword);
    m_settingListView->setKeyword(keyword);
    m_appListView->setKeyword(keyword);
    m_bestListView->setKeyword(keyword);
}

/**
 * @brief ContentWidget::setQuicklyOpenList 设置快速打开列表
 * @param list
 */
void ContentWidget::setQuicklyOpenList(const QStringList & list)
{
    m_quicklyOpenList = list;
}
