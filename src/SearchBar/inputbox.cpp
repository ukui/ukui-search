#include "inputbox.h"

/**
 * @brief ukui-search顶部搜索界面
 */
UKuiSeachBarWidget::UKuiSeachBarWidget()
{

}

UKuiSeachBarWidget::~UKuiSeachBarWidget()
{

}

/**
 * @brief ukui-search 顶部搜索框的ui，包含设置按钮
 */
UKuiSeachBar::UKuiSeachBar()
{
    setFocusPolicy(Qt::NoFocus);

    //queryWidLayout 搜索图标和文字所在的布局
//    QHBoxLayout* queryWidLayout=new QHBoxLayout;
//    this->setLayout(queryWidLayout);

//    m_queryLineEdit->setFocusPolicy(Qt::ClickFocus);
//    m_queryLineEdit->installEventFilter(this);
//    m_queryLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
//    m_queryLineEdit->setFixedSize(678,35);
//    m_queryLineEdit->setMaxLength(100);
}

UkuiSearchBarWidgetLayout::UkuiSearchBarWidgetLayout()
{
}

UkuiSearchBarWidgetLayout::~UkuiSearchBarWidgetLayout()
{

}

UKuiSeachBar::~UKuiSeachBar()
{

}

/**
 * @brief 顶部搜索框所在界面的布局
 */
UkuiSearchBarHLayout::UkuiSearchBarHLayout()
{
    initUI();
//    retouchLineEdit();

}

UkuiSearchBarHLayout::~UkuiSearchBarHLayout()
{

}

/**
 * @brief 初始化ui
 */
void UkuiSearchBarHLayout::initUI()
{
    setContentsMargins(5,3,0,2);
    setSpacing(5);
//    m_queryText->adjustSize();

//    m_queryLineEdit=new QLineEdit;
//    this->addWidget(m_queryLineEdit);
}

void UkuiSearchBarHLayout::retouchLineEdit()
{
    //输入框的搜索图标，不清楚loadSvg为什么不能用了
    QPixmap pixmap/*=loadSvg(QString(":/data/img/mainviewwidget/search.svg"),16)*/;


//    m_queryIcon=new QLabel;
//    m_queryIcon->setFixedSize(pixmap.size());
//    m_queryIcon->setPixmap(pixmap);
//    m_queryText=new QLabel;
//    m_queryText->setText(tr("Search"));
//    m_queryText->adjustSize();
//    addWidget(m_queryIcon);
//    addWidget(m_queryText);
}

UKuiSearchLineEdit::UKuiSearchLineEdit()
{

}

UKuiSearchLineEdit::~UKuiSearchLineEdit()
{

}
