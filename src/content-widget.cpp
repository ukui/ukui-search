#include "content-widget.h"
#include <QDebug>
#include <QLabel>
#include <QTimer>
#include "config-file.h"

ContentWidget::ContentWidget(QWidget * parent):QStackedWidget(parent)
{
    initUI();
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
    m_resultListArea->setStyleSheet("QScrollArea{background:transparent;}");
    m_resultDetailArea->setStyleSheet("QScrollArea{background: rgba(0,0,0,0.05); border-radius: 4px;}");
    this->addWidget(m_homePage);
    this->addWidget(m_resultPage);

    setPage(SearchItem::SearchType::All);//初始化按“全部”加载
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
    QStringList quicklyList;
    quicklyList<<"/usr/share/applications/peony.desktop"<<"/usr/share/applications/ukui-control-center.desktop"<<"Theme/主题/更改壁纸";
    lists.append(commonlyList);
    lists.append(recentlyList);
    lists.append(quicklyList);

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
                layout->addWidget(item, j / 2, j % 2);
            }
        } else {
            itemWidget->setFixedHeight(136);
            QHBoxLayout * layout = new QHBoxLayout(itemWidget);
            layout->setSpacing(8);
            layout->setContentsMargins(0, 0, 0, 0);
            itemWidget->setLayout(layout);
            Q_FOREACH(QString path, lists.at(i)){
                HomePageItem * item = new HomePageItem(itemWidget, i, path);
                layout->addWidget(item);
            }
            for (int j = 0; j < 4 - lists.at(i).length(); j++) {
                QWidget * emptyItem = new QWidget(itemWidget);
                emptyItem->setFixedSize(136, 136); //占位用widget,少于4项会补全后方占位
                layout->addWidget(emptyItem);
            }
            if (i) titleLabel->setText(tr("Open Quickly"));
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
 * @param types 获取到的搜索结果类型，仅针对有搜索结果的类型构建listview
 * @param lists 获取到的搜索结果列表（每个类型对应一个列表）
 * @param keyword 搜索关键字
 */
void ContentWidget::refreshSearchList(const QVector<int>& types, const QVector<QStringList>& lists, const QString& keyword) {
    if (!m_listLyt->isEmpty()) {
        if (m_fileListView) {
            m_fileListView->clear();
        }
        if (m_dirListView) {
            m_dirListView->clear();
        }
        if (m_contentListView) {
            m_contentListView->clear();
        }
        clearLayout(m_listLyt);
        m_resultList->setFixedHeight(0);
    }
    bool isEmpty = true;
    QStringList bestList;
    for (int i = 0; i < types.count(); i ++) {
        if (lists.at(i).isEmpty()) {
            continue;
        }
        bestList << lists.at(i).at(0);
        isEmpty = false;
        SearchListView * searchList = new SearchListView(m_resultList, lists.at(i), types.at(i), keyword); //Treeview
        QLabel * titleLabel = new QLabel(m_resultList); //表头
        titleLabel->setContentsMargins(8, 0, 0, 0);
        titleLabel->setStyleSheet("QLabel{background: rgba(0,0,0,0.1);}");
        titleLabel->setText(getTitleName(types.at(i)));
        m_listLyt->addWidget(titleLabel);
        m_listLyt->addWidget(searchList);
        m_resultList->setFixedHeight(m_resultList->height() + searchList->height() + titleLabel->height());
//        if (i == 0) {
//            searchList->setCurrentIndex(searchList->model()->index(0,1, QModelIndex()));
//            m_detailView->setupWidget(searchList->getCurrentType(), lists.at(0).at(0));
//        }
        connect(searchList, &SearchListView::currentRowChanged, this, [ = ](const int& type, const QString& path) {
            if(type == SearchItem::SearchType::Contents && !m_contentList.isEmpty()) {
                m_detailView->setContent(m_contentList.at(searchList->currentIndex().row()), keyword);
            }
            m_detailView->setupWidget(type, path);
            searchList->is_current_list = true;
            Q_EMIT this->currentItemChanged();
            searchList->is_current_list = false;
        });
        connect(this, &ContentWidget::currentItemChanged, searchList, [ = ]() {
            if (! searchList->is_current_list) {
                searchList->blockSignals(true);
                searchList->clearSelection();
                searchList->blockSignals(false);
            }
        });
    }
    if (isEmpty) {
        m_detailView->clearLayout(); //没有搜到结果，清空详情页
        return;
    }
    SearchListView * searchList = new SearchListView(m_resultList, bestList, SearchItem::SearchType::Best, keyword); //Treeview
    QLabel * titleLabel = new QLabel(m_resultList); //表头
    titleLabel->setContentsMargins(8, 0, 0, 0);
    titleLabel->setStyleSheet("QLabel{background: rgba(0,0,0,0.1);}");
    titleLabel->setText(getTitleName(SearchItem::SearchType::Best));
    m_listLyt->insertWidget(0, searchList);
    m_listLyt->insertWidget(0, titleLabel);
    m_resultList->setFixedHeight(m_resultList->height() + searchList->height() + titleLabel->height());
    searchList->setCurrentIndex(searchList->model()->index(0, 0, QModelIndex()));
    m_detailView->setupWidget(searchList->getCurrentType(), bestList.at(0));
    connect(searchList, &SearchListView::currentRowChanged, this, [ = ](const int& type, const QString& path) {
        m_detailView->setupWidget(type, path);
        searchList->is_current_list = true;
        Q_EMIT this->currentItemChanged();
        searchList->is_current_list = false;
    });
    connect(this, &ContentWidget::currentItemChanged, searchList, [ = ]() {
        if (! searchList->is_current_list) {
            searchList->blockSignals(true);
            searchList->clearSelection();
            searchList->blockSignals(false);
        }
    });
}

/**
 * @brief ContentWidget::appendSearchItem 向列表添加一项搜索结果
 * @param type 类型
 * @param path 路径
 * @param contents 文件内容
 */
void ContentWidget::appendSearchItem(const int& type, const QString& path, const QString& keyword, QStringList contents) {
    m_keyword = keyword;
    switch (type) {
        case SearchItem::SearchType::Files: {
            if (!m_fileListView) {
                m_fileListView = new SearchListView(m_resultList, QStringList(), type, keyword);
                QLabel * titleLabel = new QLabel(m_resultList); //表头
                titleLabel->setContentsMargins(8, 0, 0, 0);
                titleLabel->setStyleSheet("QLabel{background: rgba(0,0,0,0.1);}");
                titleLabel->setText(getTitleName(type));
                m_listLyt->addWidget(titleLabel);
                m_listLyt->addWidget(m_fileListView);
                connect(m_fileListView, &SearchListView::currentRowChanged, this, [ = ](const int& type, const QString& path) {
                    m_detailView->setupWidget(type, path);
                    m_fileListView->is_current_list = true;
                    Q_EMIT this->currentItemChanged();
                    m_fileListView->is_current_list = false;
                });
                connect(this, &ContentWidget::currentItemChanged, m_fileListView, [ = ]() {
                    if (! m_fileListView->is_current_list) {
                        m_fileListView->blockSignals(true);
                        m_fileListView->clearSelection();
                        m_fileListView->blockSignals(false);
                    }
                });
                m_resultList->setFixedHeight(m_resultList->height() + m_fileListView->height() + titleLabel->height());
            }
            if (m_fileListView->isHidden) {
                m_fileListView->isHidden = false;
                QLabel * titleLabel = new QLabel(m_resultList); //表头
                titleLabel->setContentsMargins(8, 0, 0, 0);
                titleLabel->setStyleSheet("QLabel{background: rgba(0,0,0,0.1);}");
                titleLabel->setText(getTitleName(type));
                m_listLyt->addWidget(titleLabel);
                m_listLyt->addWidget(m_fileListView);
                m_resultList->setFixedHeight(m_resultList->height() + m_fileListView->height() + titleLabel->height());
            }
            m_fileListView->setKeyword(keyword);
            m_fileListView->appendItem(path);
            m_resultList->setFixedHeight(m_resultList->height() + m_fileListView->rowheight);
            return;
            break;
        }
        case SearchItem::SearchType::Dirs: {
            if (!m_dirListView) {
                m_dirListView = new SearchListView(m_resultList, QStringList(), type, keyword);
                QLabel * titleLabel = new QLabel(m_resultList); //表头
                titleLabel->setContentsMargins(8, 0, 0, 0);
                titleLabel->setStyleSheet("QLabel{background: rgba(0,0,0,0.1);}");
                titleLabel->setText(getTitleName(type));
                m_listLyt->addWidget(titleLabel);
                m_listLyt->addWidget(m_dirListView);
                connect(m_dirListView, &SearchListView::currentRowChanged, this, [ = ](const int& type, const QString& path) {
                    m_detailView->setupWidget(type, path);
                    m_dirListView->is_current_list = true;
                    Q_EMIT this->currentItemChanged();
                    m_dirListView->is_current_list = false;
                });
                connect(this, &ContentWidget::currentItemChanged, m_dirListView, [ = ]() {
                    if (! m_dirListView->is_current_list) {
                        m_dirListView->blockSignals(true);
                        m_dirListView->clearSelection();
                        m_dirListView->blockSignals(false);
                    }
                });
                m_resultList->setFixedHeight(m_resultList->height() + m_dirListView->height() + titleLabel->height());
            }
            if (m_dirListView->isHidden) {
                m_dirListView->isHidden = false;
                QLabel * titleLabel = new QLabel(m_resultList); //表头
                titleLabel->setContentsMargins(8, 0, 0, 0);
                titleLabel->setStyleSheet("QLabel{background: rgba(0,0,0,0.1);}");
                titleLabel->setText(getTitleName(type));
                m_listLyt->addWidget(titleLabel);
                m_listLyt->addWidget(m_dirListView);
                m_resultList->setFixedHeight(m_resultList->height() + m_dirListView->height() + titleLabel->height());
            }
            m_dirListView->setKeyword(keyword);
            m_dirListView->appendItem(path);
            m_resultList->setFixedHeight(m_resultList->height() + m_dirListView->rowheight);
            return;
            break;
        }
        case SearchItem::SearchType::Contents: {
            if (!m_contentListView) {
                m_contentListView = new SearchListView(m_resultList, QStringList(), type, keyword);
                QLabel * titleLabel = new QLabel(m_resultList); //表头
                titleLabel->setContentsMargins(8, 0, 0, 0);
                titleLabel->setStyleSheet("QLabel{background: rgba(0,0,0,0.1);}");
                titleLabel->setText(getTitleName(type));
                m_listLyt->addWidget(titleLabel);
                m_listLyt->addWidget(m_contentListView);
                connect(m_contentListView, &SearchListView::currentRowChanged, this, [ = ](const int& type, const QString& path) {
                    m_detailView->setContent(m_contentList.at(m_contentListView->currentIndex().row()), m_keyword);
                    m_detailView->setupWidget(type, path);
                    m_contentListView->is_current_list = true;
                    Q_EMIT this->currentItemChanged();
                    m_contentListView->is_current_list = false;
                });
                connect(this, &ContentWidget::currentItemChanged, m_contentListView, [ = ]() {
                    if (! m_contentListView->is_current_list) {
                        m_contentListView->blockSignals(true);
                        m_contentListView->clearSelection();
                        m_contentListView->blockSignals(false);
                    }
                });
                m_resultList->setFixedHeight(m_resultList->height() + m_contentListView->height() + titleLabel->height());
            }
            if (m_contentListView->isHidden) {
                m_contentListView->isHidden = false;
                QLabel * titleLabel = new QLabel(m_resultList); //表头
                titleLabel->setContentsMargins(8, 0, 0, 0);
                titleLabel->setStyleSheet("QLabel{background: rgba(0,0,0,0.1);}");
                titleLabel->setText(getTitleName(type));
                m_listLyt->addWidget(titleLabel);
                m_listLyt->addWidget(m_contentListView);
                m_resultList->setFixedHeight(m_resultList->height() + m_contentListView->height() + titleLabel->height());
            }
            m_contentListView->setKeyword(keyword);
            m_contentListView->appendItem(path);
            m_resultList->setFixedHeight(m_resultList->height() + m_contentListView->rowheight);
            QString temp;
            for (int i = 0; i < contents.length(); i ++) {
                temp.append(contents.at(i));
                if (i != contents.length() - 1) {
                    temp.append("\n");
                }
            }
            m_contentList.append(temp);
            return;
            break;
        }
        default:
            break;
    }
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
    m_contentList.clear();
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
    m_contentList.clear();
    m_contentList = list;
}
