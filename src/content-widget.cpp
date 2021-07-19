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

using namespace Zeeker;
ContentWidget::ContentWidget(QWidget * parent): QStackedWidget(parent) {
    initUI();
    initListView();
    //快速入口应用列表
//    m_quicklyOpenList<<"/usr/share/applications/peony.desktop"<<"/usr/share/applications/ukui-control-center.desktop"<<"/usr/share/applications/ksc-defender.desktop";
    m_quicklyOpenList << "/usr/share/applications/ksc-defender.desktop"
                      << "/usr/share/applications/ukui-notebook.desktop"
                      << "/usr/share/applications/kylin-photo-viewer.desktop"
                      << "/usr/share/applications/pluma.desktop"
                      << "/usr/share/applications/claws-mail.desktop" ;
    if (QString::compare(FileUtils::getAppName(m_quicklyOpenList.at(2)), "Unknown App") == 0) {
        m_quicklyOpenList.replace(2, "/usr/share/applications/eom.desktop");
    }
    if (QString::compare(FileUtils::getAppName(m_quicklyOpenList.at(4)), "Unknown App") == 0) {
        m_quicklyOpenList.replace(4, "/usr/share/applications/org.gnome.Evolution.desktop");
    }
}

ContentWidget::~ContentWidget() {
    if(m_homePage) {
        delete m_homePage;
        m_homePage = nullptr;
    }
    if(m_resultPage) {
        delete m_resultPage;
        m_resultPage = nullptr;
    }
}

/**
 * @brief initUI 初始化homepage和resultpage
 */
void ContentWidget::initUI() {
    this->setFixedHeight(486);
    QPalette pal = palette();
    QPalette scroll_bar_pal = palette();
//    pal.setColor(QPalette::Base, QColor(0, 0, 0, 0));
    pal.setColor(QPalette::Window, QColor(0, 0, 0, 0)); //使用此palette的窗口背景将为透明
    scroll_bar_pal.setColor(QPalette::Base, QColor(0, 0, 0, 0));
    m_homePage = new QWidget(this);
    m_homePageLyt = new QVBoxLayout(m_homePage);
    m_homePageLyt->setSpacing(0);
    m_homePageLyt->setContentsMargins(0, 0, 0, 0);
    m_homePage->setLayout(m_homePageLyt);

    m_resultPage = new QWidget(this);
    m_resultPageLyt = new QHBoxLayout(m_resultPage);
    m_resultPageLyt->setSpacing(0);
    m_resultPageLyt->setContentsMargins(0, 0, 0, 0);
    m_resultListArea = new QScrollArea(m_resultPage);
    m_resultListArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_resultListArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_resultDetailArea = new QScrollArea(m_resultPage);
    m_resultDetailArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_resultDetailArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_resultListArea->setFixedWidth(280);
    m_resultPageLyt->addWidget(m_resultListArea);
    m_resultPageLyt->addWidget(m_resultDetailArea);
    m_resultPage->setLayout(m_resultPageLyt);

    m_resultList = new QWidget(m_resultListArea);
    m_resultDetail = new QWidget(m_resultDetailArea);
    m_listLyt = new QVBoxLayout(m_resultList);
    m_detailLyt = new QVBoxLayout(m_resultDetail);
    //需要给滚动条留出16个像素点的宽度
    m_resultList->setFixedWidth(280 - 16);
    m_resultList->setFixedHeight(0);
    m_listLyt->setContentsMargins(0, 0, 0, 0);
    m_listLyt->setSpacing(0);
    m_resultListArea->setWidget(m_resultList);
    m_resultListArea->setWidgetResizable(true);
    m_detailView = new SearchDetailView(m_resultDetailArea);
    connect(m_detailView, &SearchDetailView::configFileChanged, this, [ = ]() {
        clearLayout(m_homePageLyt);
        initHomePage();
    });
    connect(m_detailView, &SearchDetailView::actionTriggerd, this, &ContentWidget::effectiveSearch);
    m_resultDetailArea->setWidget(m_detailView);
    m_resultDetailArea->setWidgetResizable(true);
    m_resultListArea->setFrameShape(QFrame::NoFrame);
    m_resultDetailArea->setFrameShape(QFrame::NoFrame);
    m_resultListArea->setPalette(pal);
    m_resultDetailArea->setPalette(pal);
    m_resultListArea->verticalScrollBar()->setPalette(scroll_bar_pal);
    m_resultDetailArea->verticalScrollBar()->setPalette(scroll_bar_pal);
    this->addWidget(m_homePage);
    this->addWidget(m_resultPage);

    setPage(SearchItem::SearchType::Best);//初始化按“最佳”加载
}

/**
 * @brief ContentWidget::initListView 初始化所有搜索结果列表
 */
void ContentWidget::initListView() {
    m_fileListView = new SearchListView(m_resultList, QStringList(), SearchItem::SearchType::Files);
    m_dirListView = new SearchListView(m_resultList, QStringList(), SearchItem::SearchType::Dirs);
    m_contentListView = new SearchListView(m_resultList, QStringList(), SearchItem::SearchType::Contents);
    m_settingListView = new SearchListView(m_resultList, QStringList(), SearchItem::SearchType::Settings);
    m_appListView = new SearchListView(m_resultList, QStringList(), SearchItem::SearchType::Apps);
    m_bestListView = new SearchListView(m_resultList, QStringList(), SearchItem::SearchType::Best);
    m_webListView = new SearchListView(m_resultList, QStringList(), SearchItem::SearchType::Web);

    setupConnect(m_fileListView);
    setupConnect(m_dirListView);
    setupConnect(m_contentListView);
    setupConnect(m_settingListView);
    setupConnect(m_appListView);
    setupConnect(m_bestListView);
    setupConnect(m_webListView);

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
    m_webTitleLabel = new TitleLabel(m_resultList);
    m_webTitleLabel->setText(getTitleName(SearchItem::SearchType::Web));

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
    m_listLyt->addWidget(m_webTitleLabel);
    m_listLyt->addWidget(m_webListView);

    this->hideListView();
    m_resultList->setFixedHeight(0);

    m_resultListArea->setFocusProxy(m_bestListView);
    m_bestListView->setFocus();
    connect(m_appShowMoreLabel, &ShowMoreLabel::showMoreClicked, this, [ = ]() {
        m_appListView->setList(m_appList);
        m_appShowMoreLabel->stopLoading();
        this->resetListHeight();
        m_resultListArea->setFocusProxy(m_appListView);
        m_appListView->setFocus();
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
        m_resultListArea->setFocusProxy(m_settingListView);
        m_settingListView->setFocus();
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
        m_resultListArea->setFocusProxy(m_dirListView);
        m_dirListView->setFocus();
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
        m_resultListArea->setFocusProxy(m_fileListView);
        m_fileListView->setFocus();
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
        m_resultListArea->setFocusProxy(m_contentListView);
        m_contentListView->setFocus();
    });
    connect(m_contentShowMoreLabel, &ShowMoreLabel::retractClicked, this, [ = ]() {
        m_contentListView->setList(m_contentList.mid(0, 5));
        m_contentShowMoreLabel->stopLoading();
        this->resetListHeight();
    });

    connect(qApp, &QApplication::paletteChanged, this, [ = ]() {
        m_fileListView->refresh();
        m_dirListView->refresh();
        m_contentListView->refresh();
        m_settingListView->refresh();
        m_appListView->refresh();
        m_bestListView->refresh();
        m_webListView->refresh();
        this->resetListHeight();
    });
}

/**
 * @brief ContentWidget::hideListView 隐藏所有列表
 */
void ContentWidget::hideListView() {
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
    m_webTitleLabel->hide();
    m_webListView->hide();
}

/**
 * @brief ContentWidget::setupConnect 初始化各个treeview的信号与槽
 * @param listview
 */
void ContentWidget::setupConnect(SearchListView * listview) {
    connect(this, &ContentWidget::currentItemChanged, listview, [ = ]() {

        if(! listview->is_current_list) {
            listview->blockSignals(true);
            listview->clearSelection();
            listview->blockSignals(false);
        }
    });
    connect(listview, &SearchListView::currentSelectPos, [ = ](QPoint pos) {
        m_resultListArea->ensureVisible(pos.x(), pos.y());
    });
    connect(listview, &SearchListView::mousePressed, this, &ContentWidget::effectiveSearch);
    connect(listview, &SearchListView::currentRowChanged, this, &ContentWidget::onListViewRowChanged);
    connect(listview, &SearchListView::onRowDoubleClicked, this, &ContentWidget::onListViewRowDoubleClicked);
}

/**
 * @brief ContentWidget::resetHeight 重新计算列表高度
 */
void ContentWidget::resetListHeight() {
    int height = 0;
    if(! m_bestListView->isHidden) {
        height += m_bestTitleLabel->height();
        height += m_bestListView->height();
    }
    if(! m_appListView->isHidden) {
        height += m_appTitleLabel->height();
        height += m_appListView->height();
        if(m_appShowMoreLabel->isVisible()) {
            height += m_appShowMoreLabel->height();
        }
    }
    if(! m_settingListView->isHidden) {
        height += m_settingTitleLabel->height();
        height += m_settingListView->height();
        if(m_settingShowMoreLabel->isVisible()) {
            height += m_settingShowMoreLabel->height();
        }
    }
    if(! m_fileListView->isHidden) {
        height += m_fileTitleLabel->height();
        height += m_fileListView->height();
        if(m_fileShowMoreLabel->isVisible()) {
            height += m_fileShowMoreLabel->height();
        }
    }
    if(! m_dirListView->isHidden) {
        height += m_dirTitleLabel->height();
        height += m_dirListView->height();
        if(m_dirShowMoreLabel->isVisible()) {
            height += m_dirShowMoreLabel->height();
        }
    }
    if(! m_contentListView->isHidden) {
        height += m_contentTitleLabel->height();
        height += m_contentListView->height();
        if(m_contentShowMoreLabel->isVisible()) {
            height += m_contentShowMoreLabel->height();
        }
    }
    if(! m_webListView->isHidden) {
        height += m_webTitleLabel->height();
        height += m_webListView->height();
    }
    m_resultList->setFixedHeight(height);
}

void ContentWidget::appendBestItem(const int &type, const QString &path) {
    m_bestList.append(QPair<int, QString>(type, path));
    m_bestListView->appendBestItem(QPair<int, QString>(type, path));
    appendSearchItem(SearchItem::SearchType::Best, path);
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

    lists.append(m_quicklyOpenList);
    lists.append(recentlyList);
    lists.append(commonlyList);

    for(int i = 0; i < lists.count(); i++) {
        if(lists.at(i).isEmpty())
            continue;
        QWidget * listWidget = new QWidget(m_homePage);
        QVBoxLayout * itemWidgetLyt = new QVBoxLayout(listWidget);
        QLabel * titleLabel = new QLabel(listWidget);
        QWidget * itemWidget = new QWidget(listWidget);
        if(i == 1) {
            if(lists.at(i).length() <= 2) itemWidget->setFixedHeight(48);
            else itemWidget->setFixedHeight(104);
            titleLabel->setText(tr("Recently Opened"));
            QGridLayout * layout = new QGridLayout(itemWidget);
            layout->setSpacing(8);
            layout->setContentsMargins(0, 0, 0, 0);
            itemWidget->setLayout(layout);
            for(int j = 0; j < lists.at(i).count(); j++) {
                HomePageItem * item = new HomePageItem(itemWidget, i, lists.at(i).at(j));
                item->setFixedSize(312, 48);
                layout->addWidget(item, j / 2, j % 2);
            }
            if(lists.at(i).length() == 1) {
                QWidget * emptyItem = new QWidget(itemWidget);
                emptyItem->setFixedSize(300, 48); //占位用widget,只有一项时在右方补全
                layout->addWidget(emptyItem, 1, 2);
            }
        } else {
            itemWidget->setFixedHeight(116);
            QHBoxLayout * layout = new QHBoxLayout(itemWidget);
            layout->setSpacing(8);
            layout->setContentsMargins(0, 0, 0, 0);
            itemWidget->setLayout(layout);
            int shownItem = lists.at(i).length();
            Q_FOREACH(QString path, lists.at(i)) {
                if(i == 0 && QString::compare(FileUtils::getAppName(path), "Unknown App") == 0) {
                    shownItem --;
                    continue;
                }
                HomePageItem * item = new HomePageItem(itemWidget, i, path);
                item->setFixedSize(116, 116);
                layout->addWidget(item);
            }
            for(int j = 0; j < 5 - shownItem; j++) {
                QWidget * emptyItem = new QWidget(itemWidget);
                emptyItem->setFixedSize(116, 116); //占位用widget,少于5项会补全后方占位
                layout->addWidget(emptyItem);
            }
            if(i == 0 && shownItem) titleLabel->setText(tr("Open Quickly"));
            else titleLabel->setText(tr("Commonly Used"));
        }
        itemWidgetLyt->setSpacing(6);
        titleLabel->setFixedHeight(24);
        titleLabel->setContentsMargins(6,0,0,0);
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
 * @brief ContentWidget::resetSearchList 在构建新的搜索结果列表前，先重置所有控件
 */
void ContentWidget::resetSearchList() {
//    this->hideListView();
    if(m_fileListView) {
        m_fileListView->hide();
        m_fileTitleLabel->hide();
        m_fileShowMoreLabel->hide();
        m_fileListView->isHidden = true;
        m_fileListView->clear();
    }
    if(m_dirListView) {
        m_dirListView->hide();
        m_dirTitleLabel->hide();
        m_dirShowMoreLabel->hide();
        m_dirListView->isHidden = true;
        m_dirListView->clear();
    }
    if(m_contentListView) {
        m_contentListView->hide();
        m_contentTitleLabel->hide();
        m_contentShowMoreLabel->hide();
        m_contentListView->isHidden = true;
        m_contentListView->clear();
    }
    if(m_appListView) {
        m_appListView->hide();
        m_appTitleLabel->hide();
        m_appShowMoreLabel->hide();
        m_appListView->isHidden = true;
        m_appListView->clear();
    }
    if(m_settingListView) {
        m_settingListView->hide();
        m_settingTitleLabel->hide();
        m_settingShowMoreLabel->hide();
        m_settingListView->isHidden = true;
        m_settingListView->clear();
    }
    if(m_bestListView) {
        m_bestListView->hide();
        m_bestTitleLabel->hide();
        m_bestListView->isHidden = true;
        m_bestListView->clear();
    }
    if(m_webListView) {
        m_webListView->clear();
        m_webListView->appendItem(m_keyword);
        m_webTitleLabel->show();
        m_webListView->show();
        m_webListView->isHidden = false;
    }

    resetListHeight();
    m_detailView->clearLayout();
    m_contentDetailList.clear();
    m_bestContent.clear();

    m_appShowMoreLabel->resetLabel();
    m_settingShowMoreLabel->resetLabel();
    m_dirShowMoreLabel->resetLabel();
    m_fileShowMoreLabel->resetLabel();
    m_contentShowMoreLabel->resetLabel();

    m_bestList.clear();
    if(! m_appList.isEmpty())
        m_appList.clear();
    if(! m_settingList.isEmpty())
        m_settingList.clear();
    if(! m_dirList.isEmpty())
        m_dirList.clear();
    if(! m_fileList.isEmpty())
        m_fileList.clear();
    if(! m_contentList.isEmpty())
        m_contentList.clear();
    if(! m_appPathList.isEmpty())
        m_appPathList.clear();
    if(! m_appIconList.isEmpty())
        m_appIconList.clear();
    if(!m_appDescList.isEmpty())
        m_appDescList.clear();
}

/**
 * @brief ContentWidget::setSettingList 插入设置项搜索结果列表
 * @param settingList
 */
void ContentWidget::setSettingList(const QStringList & settingList) {
    if(settingList.isEmpty())
        return;
    m_settingList = settingList;
    qDebug() << "Append a best item into list: " << settingList.at(0);
    this->appendBestItem(SearchItem::SearchType::Settings, settingList.at(0));
    m_settingListView->show();
    m_settingTitleLabel->show();
    m_settingListView->isHidden = false;
    if(m_settingList.length() <= 5) {
        m_settingListView->setList(m_settingList);
    } else {
        m_settingShowMoreLabel->show();
        m_settingListView->setList(m_settingList.mid(0, 5));
    }
    this->resetListHeight();
}

/**
 * @brief ContentWidget::setAppList 插入应用搜索结果
 * @param appList QVector<namelist,pathlist,iconlist>
 */
void ContentWidget::setAppList(const QVector<QStringList>& appList) {
    if(appList.at(0).isEmpty())
        return;
    m_appList = appList.at(0);
    m_appPathList = appList.at(1);
    m_appIconList = appList.at(2);
    m_appDescList = appList.at(3);
    m_appListView->setAppList(m_appPathList, m_appIconList);
    qDebug() << "Append a best item into list: " << appList.at(0).at(0);
    SearchItemModel * model = qobject_cast<SearchItemModel *>(m_bestListView->model());
    if(appList.at(1).at(0).isEmpty() || appList.at(1).at(0) == "") {
        model->setBestAppIcon(appList.at(2).at(0), false);
    } else {
        model->setBestAppIcon(appList.at(2).at(0), true);
    }
    this->appendBestItem(SearchItem::SearchType::Apps, appList.at(0).at(0));
    m_appListView->show();
    m_appTitleLabel->show();
    m_appListView->isHidden = false;
    if(m_appList.length() <= 5) {
        m_appListView->setList(m_appList);
    } else {
        m_appShowMoreLabel->show();
        m_appListView->setList(m_appList.mid(0, 5));
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
    switch(type) {
    case SearchItem::SearchType::Best: {
        if(m_bestListView->isHidden) {
            m_bestListView->show();
            m_bestTitleLabel->show();
            m_bestListView->isHidden = false;
        }
        m_bestListView->appendItem(path);
        if(m_detailView->isEmpty()) {
            m_bestListView->setCurrentIndex(m_bestListView->model()->index(0, 0, QModelIndex()));
        }
        break;
    }
    case SearchItem::SearchType::Files: {
        if(m_fileListView->isHidden) {
            m_fileListView->show();
            m_fileTitleLabel->show();
            m_fileListView->isHidden = false;
            this->appendBestItem(SearchItem::SearchType::Files, path);
        }
        if(m_fileListView->getLength() < 5) {  //当已搜索结果列表少于5项，直接将搜索结果添加到列表中
            m_fileListView->appendItem(path);
        } else if(m_fileListView->getLength() == 5) {  //当已搜索结果等于5项，新增的被折叠，显示“展开”按钮
            m_fileShowMoreLabel->show();
        } else { //当搜索列表显示的大于5项，说明列表被展开，可以继续把新增项添加到列表中
            m_fileListView->appendItem(path);
        }
        m_fileList.append(path);
        break;;
    }
    case SearchItem::SearchType::Dirs: {
        if(m_dirListView->isHidden) {
            m_dirListView->show();
            m_dirTitleLabel->show();
            m_dirListView->isHidden = false;
            this->appendBestItem(SearchItem::SearchType::Dirs, path);
        }
        if(m_dirListView->getLength() < 5) {
            m_dirListView->appendItem(path);
        } else if(m_dirListView->getLength() == 5) {
            m_dirShowMoreLabel->show();
        } else {
            m_dirListView->appendItem(path);
        }
        m_dirList.append(path);
        break;
    }
    case SearchItem::SearchType::Contents: {
        if(m_contentListView->isHidden) {
            m_contentListView->show();
            m_contentTitleLabel->show();
            m_contentListView->isHidden = false;
            for(int i = 0; i < contents.length(); i ++) {
                m_bestContent.append(contents.at(i));
                if(i != contents.length() - 1) {
                    m_bestContent.append("\n");
                }
            }
            this->appendBestItem(SearchItem::SearchType::Contents, path);
        }
        if(m_contentListView->getLength() < 5) {
            m_contentListView->appendItem(path);
        } else if(m_contentListView->getLength() == 5) {
            m_contentShowMoreLabel->show();
        } else {
            m_contentListView->appendItem(path);
        }
        m_contentList.append(path);
        QString temp;
        for(int i = 0; i < contents.length(); i ++) {
            temp.append(contents.at(i));
            if(i != contents.length() - 1) {
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
    switch(type) {
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
    case SearchItem::SearchType::Web :
        return tr("Web Pages");
    default :
        return tr("Unknown");
    }
}

/**
 * @brief ContentWidget::clearLayout 清空某个布局
 * @param layout 需要清空的布局
 */
void ContentWidget::clearLayout(QLayout * layout) {
    if(! layout) return;
    QLayoutItem * child;
    while((child = layout->takeAt(0)) != 0) {
        if(child->widget()) {
            child->widget()->setParent(NULL); //防止删除后窗口看上去没有消失
        }
        delete child;
    }
    child = NULL;
}

/**
 * @brief ContentWidget::onListViewRowChanged 点击某列表某一行的槽函数
 * @param type
 * @param path
 */
void ContentWidget::onListViewRowChanged(SearchListView * listview, const int &type, const QString &path) {
    if(type == SearchItem::SearchType::Contents && !m_contentDetailList.isEmpty()) {
        m_detailView->isContent = true;
        m_detailView->setContent(m_contentDetailList.at(listview->currentIndex().row()), m_keyword);
    } else if(type == SearchItem::SearchType::Best && !m_bestContent.isEmpty() && SearchItem::SearchType::Contents == m_bestList.at(listview->currentIndex().row()).first) {
        m_detailView->setContent(m_bestContent, m_keyword);
        m_detailView->isContent = true;
        m_detailView->setupWidget(SearchItem::SearchType::Contents, path);
        listview->is_current_list = true;
        Q_EMIT this->currentItemChanged();
        listview->is_current_list = false;
        return;
    } else {
        m_detailView->isContent = false;
    }
    if(type == SearchItem::SearchType::Web) {
        m_detailView->setWebWidget(this->m_keyword);
    } else if(type == SearchItem::SearchType::Apps) {
        int index = listview->currentIndex().row();
        m_detailView->setAppWidget(m_appList.at(index), m_appPathList.at(index), m_appIconList.at(index), m_appDescList.at(index));
    } else if(type == SearchItem::SearchType::Best) {
        if(m_bestList.at(listview->currentIndex().row()).first ==  SearchItem::SearchType::Apps) {
            m_detailView->setAppWidget(m_appList.at(0), m_appPathList.at(0), m_appIconList.at(0), m_appDescList.at(0));
        } else {
            m_detailView->setupWidget(m_bestList.at(listview->currentIndex().row()).first, m_bestList.at(listview->currentIndex().row()).second);
        }
    } else {
        m_detailView->setupWidget(type, path);
    }
    listview->is_current_list = true;
    Q_EMIT this->currentItemChanged();
    listview->is_current_list = false;
}

/**
 * @brief ContentWidget::onListViewRowDoubleClicked 双击某列表某一行的槽函数
 * @param type
 * @param path
 */
void ContentWidget::onListViewRowDoubleClicked(SearchListView * listview, const int &type, const QString &path) {
    qDebug() << "A row has been double clicked.Type = " << type << "; Name = " << path;
    if(type == SearchItem::SearchType::Best && m_bestList.at(listview->currentIndex().row()).first != SearchItem::SearchType::Apps) {
        m_detailView->doubleClickAction(m_bestList.at(listview->currentIndex().row()).first, path);
    } else if(type == SearchItem::SearchType::Best && m_bestList.at(listview->currentIndex().row()).first == SearchItem::SearchType::Apps) {
        if(m_appPathList.at(0) == "" || m_appPathList.at(0).isEmpty()) {
            m_detailView->doubleClickAction(SearchListView::ResType::App, m_appList.at(0));
        } else {
            m_detailView->doubleClickAction(SearchListView::ResType::App, m_appPathList.at(0));
        }
    } else if(type == SearchItem::SearchType::Apps) {
        int index = listview->currentIndex().row();
        if(m_appPathList.at(index) == "" || m_appPathList.at(index).isEmpty()) {
            m_detailView->doubleClickAction(SearchListView::ResType::App, m_appList.at(index));
        } else {
            m_detailView->doubleClickAction(SearchListView::ResType::App, m_appPathList.at(index));
        }
    } else {
        m_detailView->doubleClickAction(type, path);
    }
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
void ContentWidget::setKeyword(QString keyword) {
    m_keyword = keyword;
    m_fileListView->setKeyword(keyword);
    m_dirListView->setKeyword(keyword);
    m_contentListView->setKeyword(keyword);
    m_settingListView->setKeyword(keyword);
    m_appListView->setKeyword(keyword);
    m_bestListView->setKeyword(keyword);
    m_webListView->setKeyword(keyword);
}

/**
 * @brief ContentWidget::setQuicklyOpenList 设置快速打开列表
 * @param list
 */
void ContentWidget::setQuicklyOpenList(const QStringList & list) {
    m_quicklyOpenList = list;
}

/**
 * @brief ContentWidget::closeWebView 在主界面失焦消失的时候调用，（若webview未关闭）关闭网页搜索界面
 */
void ContentWidget::closeWebView() {
    m_detailView->closeWebWidget();
}
