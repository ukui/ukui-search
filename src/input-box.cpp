#include "input-box.h"

/**
 * @brief ukui-search顶部搜索界面
 */
UKuiSeachBarWidget::UKuiSeachBarWidget()
{
//    this->setFixedSize(Style::defaultMainViewWidWidth,Style::defaultTopWidHeight);

}

UKuiSeachBarWidget::~UKuiSeachBarWidget()
{

}

void UKuiSeachBarWidget::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    QRect rect = this->rect();
    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    p.setBrush(this->palette().color(QPalette::Base));
//    p.setBrush(QBrush(QColor(255,255,255)));
    p.setOpacity(1);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rect,12,12);
    QWidget::paintEvent(e);
}

/**
 * @brief ukui-search 顶部搜索框的ui，包含设置按钮
 */
UKuiSeachBar::UKuiSeachBar()
{
    setFocusPolicy(Qt::NoFocus);
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

//    connect(m_queryLineEdit, SIGNAL(textChanged(QString)), SIGNAL(textChanged(QString)));
    m_timer = new QTimer;
    connect(m_timer, &QTimer::timeout, this, [ = ](){
        m_timer->stop();
        Q_EMIT this->textChanged(m_queryLineEdit->text());
    });
    connect(m_queryLineEdit, &UKuiSearchLineEdit::textChanged, this, [ = ](QString text) {
        if (m_isEmpty) {
            m_isEmpty = false;
            Q_EMIT this->textChanged(text);
        } else {
            if (text == "") {
                m_isEmpty = true;
                m_timer->stop();
                Q_EMIT this->textChanged(m_queryLineEdit->text());
                return;
            }
            m_timer->stop();
            m_timer->start(0.2 * 1000);
        }
    });
}

UkuiSearchBarHLayout::~UkuiSearchBarHLayout()
{
    if (m_timer) {
        delete m_timer;
        m_timer = NULL;
    }
}

/**
 * @brief 初始化ui
 */
void UkuiSearchBarHLayout::initUI()
{
    setContentsMargins(5,3,0,2);
    setSpacing(5);
}

void UkuiSearchBarHLayout::searchContent(QString searchcontent){
    m_queryLineEdit->setText(searchcontent);
}

void UkuiSearchBarHLayout::clearText() {
    m_queryLineEdit->setText("");
}

/**
 * @brief UKuiSearchLineEdit  全局搜索的输入框
 */
UKuiSearchLineEdit::UKuiSearchLineEdit()
{
    this->setFocusPolicy(Qt::ClickFocus);
    this->installEventFilter(this);
    this->setContextMenuPolicy(Qt::NoContextMenu);
    this->setMaxLength(100);


    QAction *searchAction = new QAction(this);
    searchAction->setIcon(QIcon(":/res/icons/edit-find-symbolic.svg"));
    this->addAction(searchAction,QLineEdit::LeadingPosition);



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
