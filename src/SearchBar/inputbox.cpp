#include "inputbox.h"
#include "src/Style/style.h"
/**
 * @brief ukui-search顶部搜索界面
 */
UKuiSeachBarWidget::UKuiSeachBarWidget()
{
    this->setFixedSize(Style::defaultMainViewWidWidth,Style::defaultTopWidHeight);
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

        m_queryLineEdit=new UKuiSearchLineEdit;

    this->setContentsMargins(0,0,0,0);
    this->setAlignment(m_queryLineEdit,Qt::AlignCenter);


    this->addWidget(m_queryLineEdit);


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

/**
 * @brief UKuiSearchLineEdit  全局搜索的输入框
 */
UKuiSearchLineEdit::UKuiSearchLineEdit()
{
    this->setFocusPolicy(Qt::ClickFocus);
    this->installEventFilter(this);
    this->setContextMenuPolicy(Qt::NoContextMenu);
    this->setFixedSize(678,35);
    this->setMaxLength(100);

    /*发送输入框文字改变的dbus*/
    QDBusConnection::sessionBus().unregisterService("org.ukui.search.service");
    QDBusConnection::sessionBus().registerService("org.ukui.search.service");
    QDBusConnection::sessionBus().registerObject("/lineEdit/textChanged", this,QDBusConnection :: ExportAllSlots | QDBusConnection :: ExportAllSignals);

    connect(this, &QLineEdit::textChanged, this, &UKuiSearchLineEdit::lineEditTextChanged);
}

UKuiSearchLineEdit::~UKuiSearchLineEdit()
{

}

/**
 * @brief lineEditTextChange 监听到搜索框文字的textChanged信号，发送dbus信号给其他程序
 * @param arg 搜索框的文本
 *
 * 需要此点击信号的应用需要做如下绑定
 * QDBusConnection::sessionBus().connect(QString(), QString("/lineEdit/textChanged"), "org.ukui.search.inputbox", "InputBoxTextChanged", this, SLOT(client_get(QString)));
 * 在槽函数client_get(void)　中处理接受到的点击信号
 */
void UKuiSearchLineEdit::lineEditTextChanged(QString arg)
{
    QDBusMessage message =QDBusMessage::createSignal("/lineEdit/textChanged", "org.ukui.search.inputbox", "InputBoxTextChanged");
    message<<arg;
    QDBusConnection::sessionBus().send(message);
}
