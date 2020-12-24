#include "content-widget.h"
#include <QDebug>
#include <QLabel>

ContentWidget::ContentWidget(QWidget * parent):QStackedWidget(parent)
{
    initUI();
    QVector<int> types;
    QVector<QStringList> lists;
    QStringList list;
    list<<"/usr/share/applications/code.desktop"<<"/usr/share/applications/fcitx.desktop"<<"/usr/share/applications/peony.desktop"<<"/usr/share/applications/info.desktop"<<"/usr/share/applications/yelp.desktop";
    QStringList list2;
    list2<<"/home/zjp/下载/搜索结果.png"<<"/home/zjp/下载/显示不全.mp4"<<"/home/zjp/下载/u=1747586012,2959413014&fm=26&gp=0.jpg"<<"/home/zjp/下载/dmesg.log"<<"/home/zjp/下载/WiFi_AP选择.docx";
    QStringList list3;
    list3<<"About/设置->功能点"<<"Area/设置->功能点"<<"Datetime/设置->功能点"<<"Theme/设置->功能点"<<"233/设置->功能点";
    types.append(SearchItem::SearchType::Apps);
    types.append(SearchItem::SearchType::Files);
    types.append(SearchItem::SearchType::Settings);
    lists.append(list);
    lists.append(list2);
    lists.append(list3);
    refreshSearchList(types, lists);
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
 * @brief initUI
 * 初始化homepage和resultpage
 */
void ContentWidget::initUI() {
    m_homePage = new QWidget;
    m_homePageLyt = new QVBoxLayout(m_homePage);
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
    m_resultListArea->setFixedWidth(240);
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
    m_listLyt->setContentsMargins(0, 0, 20, 0);
    m_listLyt->setSpacing(0);
    m_resultListArea->setWidget(m_resultList);
    m_resultListArea->setWidgetResizable(true);
    m_homePage->setStyleSheet("QWidget{background:pink;}");
    m_resultListArea->setStyleSheet("QScrollArea{background:transparent;}");
    m_resultDetailArea->setStyleSheet("QScrollArea{background:yellow;}");
    this->addWidget(m_homePage);
    this->addWidget(m_resultPage);

    setPageType(SearchItem::SearchType::All);//初始化按“全部”加载
}

/**
 * @brief setPageType 预留的接口，为指定类别搜索调整界面内容
 * @param type
 */
void ContentWidget::setPageType(const int& type){
    m_currentType = type;
}

/**
 * @brief ContentWidget::currentType 返回当前内容页（home或searchresult）
 * @return
 */
int ContentWidget::currentType() {
    return m_currentType;
}

/**
 * @brief ContentWidget::refreshSearchList 刷新/构建搜索结果列表
 * @param types 获取到的搜索结果类型，仅针对有搜索结果的类型构建listview
 * @param lists 获取到的搜索结果列表（每个类型对应一个列表）
 */
void ContentWidget::refreshSearchList(const QVector<int>& types, const QVector<QStringList>& lists) {
    if (!m_listLyt->isEmpty()) {
        clearSearchList();
    }
    for (int i = 0; i < types.count(); i ++) {
        SearchListView * searchList = new SearchListView(m_resultList, lists.at(i), types.at(i)); //Treeview
        QLabel * titleLabel = new QLabel(m_resultList); //表头
        titleLabel->setContentsMargins(8, 0, 0, 0);
        titleLabel->setStyleSheet("QLabel{background: rgba(0,0,0,0.1)}");
        titleLabel->setText(getTitleName(types.at(i)));
        m_listLyt->addWidget(titleLabel);
        m_listLyt->addWidget(searchList);
        m_resultList->setFixedHeight(m_resultList->height() + searchList->height() + titleLabel->height());
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
        case SearchItem::SearchType::Best :
            return tr("Best Matches");
        default :
            return tr("Unknown");
    }
}

/**
 * @brief ContentWidget::clearSearchList 清空搜索结果列表
 */
void ContentWidget::clearSearchList() {
    QLayoutItem * child;
    while ((child = m_listLyt->takeAt(0)) != 0) {
        if(child->widget())
        {
            child->widget()->setParent(NULL); //防止删除后窗口看上去没有消失
        }
        delete child;
    }
    child = NULL;
    m_resultList->setFixedHeight(0);
}
