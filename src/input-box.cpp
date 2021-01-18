#include "input-box.h"

/**
 * @brief ukui-search顶部搜索界面
 */
SeachBarWidget::SeachBarWidget()
{
}

SeachBarWidget::~SeachBarWidget()
{
}

/**
 * @brief ukui-search 顶部搜索框的ui，包含设置按钮
 */
SeachBar::SeachBar()
{
    setFocusPolicy(Qt::NoFocus);
}

SearchBarWidgetLayout::SearchBarWidgetLayout()
{
}

SearchBarWidgetLayout::~SearchBarWidgetLayout()
{
}

SeachBar::~SeachBar()
{
}

/**
 * @brief 顶部搜索框所在界面的布局
 */
SearchBarHLayout::SearchBarHLayout()
{
    initUI();

    m_timer = new QTimer;
    connect(m_timer, &QTimer::timeout, this, [ = ](){
        m_timer->stop();
        Q_EMIT this->textChanged(m_queryLineEdit->text());
    });
    connect(m_queryLineEdit, &SearchLineEdit::textChanged, this, [ = ](QString text) {
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
            m_timer->start(0.1 * 1000);
        }
    });
}

SearchBarHLayout::~SearchBarHLayout()
{
    if (m_timer) {
        delete m_timer;
        m_timer = NULL;
    }
}

/**
 * @brief 初始化ui
 */
void SearchBarHLayout::initUI()
{
    m_queryLineEdit = new SearchLineEdit;
    m_queryLineEdit->installEventFilter(this);
    m_queryLineEdit->setTextMargins(30,1,0,1);
    this->setContentsMargins(0,0,0,0);
    this->setAlignment(m_queryLineEdit,Qt::AlignCenter);
    this->addWidget(m_queryLineEdit);
    m_queryWidget = new QWidget(m_queryLineEdit);
    m_queryWidget->setFocusPolicy(Qt::NoFocus);
    m_queryWidget->setStyleSheet("border:0px;background:transparent");

    QHBoxLayout* queryWidLayout= new QHBoxLayout;
    queryWidLayout->setContentsMargins(4,0,0,0);
    queryWidLayout->setAlignment(Qt::AlignJustify);
    queryWidLayout->setSpacing(5);
    m_queryWidget->setLayout(queryWidLayout);


    QPixmap pixmap(QString(":/res/icons/edit-find-symbolic.svg"));
    m_queryIcon = new QLabel;
    m_queryIcon->setStyleSheet("background:transparent");
    m_queryIcon->setFixedSize(pixmap.size());
    m_queryIcon->setPixmap(pixmap);

    m_queryText = new QLabel;
    m_queryText->setText(tr("Search"));
    m_queryText->setStyleSheet("background:transparent;color:#626c6e;");
    m_queryText->setContentsMargins(0,0,0,4);
    m_queryText->adjustSize();

    queryWidLayout->addWidget(m_queryIcon);
    queryWidLayout->addWidget(m_queryText);
    m_queryWidget->setGeometry(QRect((m_queryLineEdit->width() - (m_queryIcon->width() + m_queryText->width() + 15)) / 2 - 10, 0,
                                     m_queryIcon->width() + m_queryText->width() + 10, 35)); //设置图标初始位置

    m_animation= new QPropertyAnimation(m_queryWidget,"geometry");
    m_animation->setDuration(100); //动画时长
    connect(m_animation,&QPropertyAnimation::finished,this, [ = ]() {
        if (m_isSearching) {
            m_queryWidget->layout()->removeWidget(m_queryText);
            m_queryText->setParent(nullptr);
        } else {
            m_queryWidget->layout()->addWidget(m_queryText);
        }
    });
}

void SearchBarHLayout::focusIn() {
    m_queryLineEdit->setFocus();
}

void SearchBarHLayout::focusOut() {
    m_queryLineEdit->clearFocus();
    if (! m_queryText->parent()) {
        m_queryWidget->layout()->addWidget(m_queryText);
        m_queryText->adjustSize();
    }
    m_queryWidget->setGeometry(QRect((m_queryLineEdit->width() - (m_queryIcon->width() + m_queryText->width() + 15)) / 2 - 10, 0,
                                     m_queryIcon->width() + m_queryText->width() + 10, 35)); //使图标回到初始位置
}

void SearchBarHLayout::clearText() {
    m_queryLineEdit->setText("");
}

QString SearchBarHLayout::text() {
    return m_queryLineEdit->text();
}

bool SearchBarHLayout::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_queryLineEdit) {
        if (event->type()==QEvent::FocusIn) {
             if (m_queryLineEdit->text().isEmpty()) {
                 m_animation->stop();
                 m_animation->setStartValue(m_queryWidget->geometry());
                 m_animation->setEndValue(QRect(0, 0, m_queryIcon->width() + 5, 35));
                 m_animation->setEasingCurve(QEasingCurve::OutQuad);
                 m_animation->start();
             }
             m_isSearching=true;
        } else if (event->type()==QEvent::FocusOut) {
            if (m_queryLineEdit->text().isEmpty()) {
                if (m_isSearching) {
                    m_animation->stop();
                    m_queryText->adjustSize();
                    m_animation->setStartValue(QRect(0, 0, m_queryIcon->width() + 5, 35));
                    m_animation->setEndValue(QRect((m_queryLineEdit->width() - (m_queryIcon->width() + m_queryText->width() + 10)) / 2, 0,
                                                 m_queryIcon->width() + m_queryText->width() + 10, 35));
                    m_animation->setEasingCurve(QEasingCurve::InQuad);
                    m_animation->start();
                }
            }
            m_isSearching=false;
        }
    }
    return QObject::eventFilter(watched, event);
}

/**
 * @brief UKuiSearchLineEdit  全局搜索的输入框
 */
SearchLineEdit::SearchLineEdit()
{
    this->setFocusPolicy(Qt::ClickFocus);
    this->installEventFilter(this);
    this->setContextMenuPolicy(Qt::NoContextMenu);
    this->setMaxLength(100);


    //这是搜索框图标，要改
//    QAction *searchAction = new QAction(this);
//    searchAction->setIcon(QIcon(":/res/icons/edit-find-symbolic.svg"));
//    this->addAction(searchAction,QLineEdit::LeadingPosition);

    /*发送输入框文字改变的dbus*/
    QDBusConnection::sessionBus().unregisterService("org.ukui.search.service");
    QDBusConnection::sessionBus().registerService("org.ukui.search.service");
    QDBusConnection::sessionBus().registerObject("/lineEdit/textChanged", this,QDBusConnection :: ExportAllSlots | QDBusConnection :: ExportAllSignals);

    connect(this, &QLineEdit::textChanged, this, &SearchLineEdit::lineEditTextChanged);
}

SearchLineEdit::~SearchLineEdit()
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
void SearchLineEdit::lineEditTextChanged(QString arg)
{
    QDBusMessage message = QDBusMessage::createSignal("/lineEdit/textChanged", "org.ukui.search.inputbox", "InputBoxTextChanged");
    message<<arg;
    QDBusConnection::sessionBus().send(message);
}
