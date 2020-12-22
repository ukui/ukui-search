#include "content-widget.h"

ContentWidget::ContentWidget(QWidget * parent):QStackedWidget(parent)
{
    initUI();
}

ContentWidget::~ContentWidget()
{
    if (homePage) {
        delete homePage;
        homePage = NULL;
    }
    if (resultPage) {
        delete resultPage;
        resultPage = NULL;
    }
}

/**
 * @brief initUI
 * 初始化homepage和resultpage
 */
void ContentWidget::initUI() {
    homePage = new QWidget;
    homePageLyt = new QVBoxLayout(homePage);
    homePage->setLayout(homePageLyt);

    resultPage = new QWidget;
    resultPageLyt = new QHBoxLayout(resultPage);
    resultPageLyt->setSpacing(0);
    resultPageLyt->setContentsMargins(0, 0, 0, 0);
    resultListArea = new QScrollArea(resultPage);
    resultListArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    resultListArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    resultDetailArea = new QScrollArea(resultPage);
    resultDetailArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    resultDetailArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    resultListArea->setFixedWidth(240);
    resultPageLyt->addWidget(resultListArea);
    resultPageLyt->addWidget(resultDetailArea);
    resultPage->setLayout(resultPageLyt);

    homePage->setStyleSheet("QWidget{background:pink;}");
    resultListArea->setStyleSheet("QScrollArea{background:blue;}");
    resultDetailArea->setStyleSheet("QScrollArea{background:yellow;}");
    this->addWidget(homePage);
    this->addWidget(resultPage);

    setPageType(SearchType::All);//初始化按“全部”加载
}

/**
 * @brief setPageType 预留的接口，为指定类别搜索调整界面内容
 * @param type
 */
void ContentWidget::setPageType(int type){
    m_current_type = type;
}

int ContentWidget::currentType() {
    return m_current_type;
}
