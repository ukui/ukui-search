#include "content-widget.h"
#include <QDebug>
#include <QLabel>
#include <QTimer>
#include "config-file.h"

ContentWidget::ContentWidget(QWidget * parent):QStackedWidget(parent)
{
    initUI();
    initListView();
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

    m_fileTitleLabel = new QLabel(m_resultList);
    m_fileTitleLabel->setContentsMargins(8, 0, 0, 0);
    m_fileTitleLabel->setStyleSheet("QLabel{background: rgba(0,0,0,0.1);}");
    m_fileTitleLabel->setText(getTitleName(SearchItem::SearchType::Files));
    m_dirTitleLabel = new QLabel(m_resultList);
    m_dirTitleLabel->setContentsMargins(8, 0, 0, 0);
    m_dirTitleLabel->setStyleSheet("QLabel{background: rgba(0,0,0,0.1);}");
    m_dirTitleLabel->setText(getTitleName(SearchItem::SearchType::Dirs));
    m_contentTitleLabel = new QLabel(m_resultList);
    m_contentTitleLabel->setContentsMargins(8, 0, 0, 0);
    m_contentTitleLabel->setStyleSheet("QLabel{background: rgba(0,0,0,0.1);}");
    m_contentTitleLabel->setText(getTitleName(SearchItem::SearchType::Contents));
    m_appTitleLabel = new QLabel(m_resultList);
    m_appTitleLabel->setContentsMargins(8, 0, 0, 0);
    m_appTitleLabel->setStyleSheet("QLabel{background: rgba(0,0,0,0.1);}");
    m_appTitleLabel->setText(getTitleName(SearchItem::SearchType::Apps));
    m_settingTitleLabel = new QLabel(m_resultList);
    m_settingTitleLabel->setContentsMargins(8, 0, 0, 0);
    m_settingTitleLabel->setStyleSheet("QLabel{background: rgba(0,0,0,0.1);}");
    m_settingTitleLabel->setText(getTitleName(SearchItem::SearchType::Settings));
    m_bestTitleLabel = new QLabel(m_resultList);
    m_bestTitleLabel->setContentsMargins(8, 0, 0, 0);
    m_bestTitleLabel->setStyleSheet("QLabel{background: rgba(0,0,0,0.1);}");
    m_bestTitleLabel->setText(getTitleName(SearchItem::SearchType::Best));

    m_listLyt->addWidget(m_bestTitleLabel);
    m_listLyt->addWidget(m_bestListView);
    m_listLyt->addWidget(m_appTitleLabel);
    m_listLyt->addWidget(m_appListView);
    m_listLyt->addWidget(m_settingTitleLabel);
    m_listLyt->addWidget(m_settingListView);
    m_listLyt->addWidget(m_dirTitleLabel);
    m_listLyt->addWidget(m_dirListView);
    m_listLyt->addWidget(m_fileTitleLabel);
    m_listLyt->addWidget(m_fileListView);
    m_listLyt->addWidget(m_contentTitleLabel);
    m_listLyt->addWidget(m_contentListView);

    this->hideListView();
    m_resultList->setFixedHeight(0);
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
    m_settingTitleLabel->hide();
    m_settingListView->hide();
    m_dirTitleLabel->hide();
    m_dirListView->hide();
    m_fileTitleLabel->hide();
    m_fileListView->hide();
    m_contentTitleLabel->hide();
    m_contentListView->hide();
}

/**
 * @brief ContentWidget::setupConnect 初始化各个treeview的信号与槽
 * @param listview
 */
void ContentWidget::setupConnect(SearchListView * listview) {
    connect(listview, &SearchListView::currentRowChanged, this, [ = ](const int& type, const QString& path) {
        if(type == SearchItem::SearchType::Contents && !m_contentList.isEmpty()) {
            m_detailView->setContent(m_contentList.at(listview->currentIndex().row()), m_keyword);
        }
        m_detailView->setupWidget(type, path);
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
    }
    if (! m_settingListView->isHidden) {
        height += m_settingTitleLabel->height();
        height += m_settingListView->height();
    }
    if (! m_fileListView->isHidden) {
        height += m_fileTitleLabel->height();
        height += m_fileListView->height();
    }
    if (! m_dirListView->isHidden) {
        height += m_dirTitleLabel->height();
        height += m_dirListView->height();
    }
    if (! m_contentListView->isHidden) {
        height += m_contentTitleLabel->height();
        height += m_contentListView->height();
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
            for (int j = 0; j < 5 - lists.at(i).length(); j++) {
                QWidget * emptyItem = new QWidget(itemWidget);
                emptyItem->setFixedSize(108, 108); //占位用widget,少于4项会补全后方占位
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
 * @param lists 获取到的应用与设置结果列表，list.at(0)是应用， list.at(1)是设置
 */
void ContentWidget::refreshSearchList(const QVector<QStringList>& lists) {
    this->hideListView();
    if (m_fileListView) {
        m_fileListView->hide();
        m_fileTitleLabel->hide();
        m_fileListView->isHidden = true;
        m_fileListView->clear();
    }
    if (m_dirListView) {
        m_dirListView->hide();
        m_dirTitleLabel->hide();
        m_dirListView->isHidden = true;
        m_dirListView->clear();
    }
    if (m_contentListView) {
        m_contentListView->hide();
        m_contentTitleLabel->hide();
        m_contentListView->isHidden = true;
        m_contentListView->clear();
    }
    if (m_appListView) {
        m_appListView->hide();
        m_appTitleLabel->hide();
        m_appListView->isHidden = true;
        m_appListView->clear();
    }
    if (m_settingListView) {
        m_settingListView->hide();
        m_settingTitleLabel->hide();
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

    if (!lists.at(0).isEmpty()) {
        qDebug()<<"Append a best item into list: "<<lists.at(0).at(0);
        appendSearchItem(SearchItem::SearchType::Best, lists.at(0).at(0));
    }
    if (!lists.at(1).isEmpty()) {
        qDebug()<<"Append a best item into list: "<<lists.at(1).at(0);
        appendSearchItem(SearchItem::SearchType::Best, lists.at(1).at(0));
    }
    if (!lists.at(0).isEmpty()) {
        m_appListView->show();
        m_appTitleLabel->show();
        m_appListView->isHidden = false;
        m_appListView->appendList(lists.at(0));
    }
    if (!lists.at(1).isEmpty()) {
        m_settingListView->show();
        m_settingTitleLabel->show();
        m_settingListView->isHidden = false;
        m_settingListView->appendList(lists.at(1));
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
            m_fileListView->appendItem(path);
            break;;
        }
        case SearchItem::SearchType::Dirs: {
            if (m_dirListView->isHidden) {
                m_dirListView->show();
                m_dirTitleLabel->show();
                m_dirListView->isHidden = false;
                appendSearchItem(SearchItem::SearchType::Best, path);
            }
            m_dirListView->appendItem(path);
            break;
        }
        case SearchItem::SearchType::Contents: {
            if (m_contentListView->isHidden) {
                m_contentListView->show();
                m_contentTitleLabel->show();
                m_contentListView->isHidden = false;
                appendSearchItem(SearchItem::SearchType::Best, path);
            }
            m_contentListView->appendItem(path);
            QString temp;
            for (int i = 0; i < contents.length(); i ++) {
                temp.append(contents.at(i));
                if (i != contents.length() - 1) {
                    temp.append("\n");
                }
            }
            m_contentList.append(temp);
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
