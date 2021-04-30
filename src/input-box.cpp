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
#include "input-box.h"

using namespace Zeeker;
/**
 * @brief ukui-search顶部搜索界面
 */
SeachBarWidget::SeachBarWidget(QWidget *parent): QWidget(parent) {
}

SeachBarWidget::~SeachBarWidget() {
}

/**
 * @brief ukui-search 顶部搜索框的ui，包含设置按钮
 */
SeachBar::SeachBar() {
    setFocusPolicy(Qt::NoFocus);
}

SearchBarWidgetLayout::SearchBarWidgetLayout() {
}

SearchBarWidgetLayout::~SearchBarWidgetLayout() {
}

SeachBar::~SeachBar() {
}

/**
 * @brief 顶部搜索框所在界面的布局
 */
SearchBarHLayout::SearchBarHLayout(QWidget *parent): QHBoxLayout(parent) {
    initUI();

    m_timer = new QTimer;
    connect(m_timer, &QTimer::timeout, this, [ = ]() {
        m_timer->stop();
        Q_EMIT this->textChanged(m_queryLineEdit->text());
    });
    connect(m_queryLineEdit, &SearchLineEdit::textChanged, this, [ = ](QString text) {
        if(m_isEmpty) {
            m_isEmpty = false;
            Q_EMIT this->textChanged(text);
        } else {
            if(text == "") {
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

SearchBarHLayout::~SearchBarHLayout() {
    if(m_timer) {
        delete m_timer;
        m_timer = NULL;
    }
    if(m_queryLineEdit) {
        delete m_queryLineEdit;
        m_queryLineEdit = nullptr;
    }
}

/**
 * @brief 初始化ui
 */
void SearchBarHLayout::initUI() {
    m_queryLineEdit = new SearchLineEdit();
    m_queryLineEdit->installEventFilter(this);
    m_queryLineEdit->setTextMargins(30, 1, 0, 1);
    this->setContentsMargins(0, 0, 0, 0);
    this->setAlignment(m_queryLineEdit, Qt::AlignCenter);
    this->addWidget(m_queryLineEdit);

    m_queryWidget = new QWidget(m_queryLineEdit);
    m_queryWidget->setFocusPolicy(Qt::NoFocus);
    m_queryWidget->setStyleSheet("border:0px;background:transparent");

    QHBoxLayout* queryWidLayout = new QHBoxLayout;
    queryWidLayout->setContentsMargins(8, 4, 0, 0);
    queryWidLayout->setAlignment(Qt::AlignJustify);
    queryWidLayout->setSpacing(5);
    m_queryWidget->setLayout(queryWidLayout);


    QPixmap pixmap(QIcon::fromTheme("system-search-symbolic").pixmap(QSize(20, 20)));
    m_queryIcon = new QLabel;
    m_queryIcon->setStyleSheet("background:transparent");
    m_queryIcon->setFixedSize(pixmap.size());
    m_queryIcon->setPixmap(pixmap);

    m_queryText = new QLabel;
    m_queryText->setText(tr("Search"));
    m_queryText->setStyleSheet("background:transparent;color:#626c6e;");
    m_queryText->setContentsMargins(0, 0, 0, 4);
    m_queryText->adjustSize();

    queryWidLayout->addWidget(m_queryIcon);
    queryWidLayout->addWidget(m_queryText);
    m_queryWidget->setGeometry(QRect((m_queryLineEdit->width() - (m_queryIcon->width() + m_queryText->width() + 15)) / 2 - 10, 0,
                                     m_queryIcon->width() + m_queryText->width() + 20, 35)); //设置图标初始位置

    m_animation = new QPropertyAnimation(m_queryWidget, "geometry");
    m_animation->setDuration(100); //动画时长
    connect(m_animation, &QPropertyAnimation::finished, this, [ = ]() {
        if(m_isSearching) {
            m_queryWidget->layout()->removeWidget(m_queryText);
            m_queryText->setParent(nullptr);
        } else {
            m_queryWidget->layout()->addWidget(m_queryText);
        }
    });
}

void SearchBarHLayout::effectiveSearchRecord() {
    m_queryLineEdit->record();
}

void SearchBarHLayout::focusIn() {
    m_queryLineEdit->setFocus();
}

void SearchBarHLayout::focusOut() {
    m_queryLineEdit->clearFocus();
    if(! m_queryText->parent()) {
        m_queryWidget->layout()->addWidget(m_queryText);
        m_queryText->adjustSize();
    }
    m_queryWidget->setGeometry(QRect((m_queryLineEdit->width() - (m_queryIcon->width() + m_queryText->width() + 15)) / 2 - 10, 0,
                                     m_queryIcon->width() + m_queryText->width() + 20, 35)); //使图标回到初始位置
}

void SearchBarHLayout::clearText() {
    m_queryLineEdit->setText("");
}

QString SearchBarHLayout::text() {
    return m_queryLineEdit->text();
}

bool SearchBarHLayout::eventFilter(QObject *watched, QEvent *event) {
    if(watched == m_queryLineEdit) {
        if(event->type() == QEvent::FocusIn) {
            if(m_queryLineEdit->text().isEmpty()) {
                m_animation->stop();
                m_animation->setStartValue(m_queryWidget->geometry());
                m_animation->setEndValue(QRect(0, 0, m_queryIcon->width() + 10, 35));
                m_animation->setEasingCurve(QEasingCurve::OutQuad);
                m_animation->start();
            }
            m_isSearching = true;
        } else if(event->type() == QEvent::FocusOut) {
            if(m_queryLineEdit->text().isEmpty()) {
                if(m_isSearching) {
                    m_animation->stop();
                    m_queryText->adjustSize();
                    m_animation->setStartValue(QRect(0, 0, m_queryIcon->width() + 5, 35));
                    m_animation->setEndValue(QRect((m_queryLineEdit->width() - (m_queryIcon->width() + m_queryText->width() + 10)) / 2, 0,
                                                   m_queryIcon->width() + m_queryText->width() + 20, 35));
                    m_animation->setEasingCurve(QEasingCurve::InQuad);
                    m_animation->start();
                }
            }
            m_isSearching = false;
        }
    }
    return QObject::eventFilter(watched, event);
}

/**
 * @brief UKuiSearchLineEdit  全局搜索的输入框
 */
SearchLineEdit::SearchLineEdit() {
    this->setFocusPolicy(Qt::ClickFocus);
    this->installEventFilter(this);
//    this->setContextMenuPolicy(Qt::NoContextMenu);
    this->setMaxLength(100);

    m_completer = new QCompleter(this);
    m_model = new QStringListModel(this);
    m_model->setStringList(GlobalSettings::getInstance()->getSearchRecord());
    m_completer->setModel(m_model);
    m_completer->setCompletionMode(QCompleter::InlineCompletion);
    //TODO make a popup window to show the completer.
//    QListView *popView = new QListView(this);
//    popView->setFocusPolicy(Qt::NoFocus);
//    popView->setProperty("useCustomShadow", true);
//    popView->setProperty("customShadowDarkness", 0.5);
//    popView->setProperty("customShadowWidth", 20);
//    popView->setProperty("customShadowRadius", QVector4D(6, 6, 6, 6));
//    popView->setProperty("customShadowMargins", QVector4D(20, 20, 20, 20));
//    popView->setAttribute(Qt::WA_TranslucentBackground);
//    m_completer->setPopup(popView);
//    m_completer->popup()->setStyle(CustomStyle::getStyle());
    m_completer->setMaxVisibleItems(14);

    setCompleter(m_completer);

    //这是搜索框图标，要改
//    QAction *searchAction = new QAction(this);
//    searchAction->setIcon(QIcon(":/res/icons/edit-find-symbolic.svg"));
//    this->addAction(searchAction,QLineEdit::LeadingPosition);

    /*发送输入框文字改变的dbus*/
    QDBusConnection::sessionBus().unregisterService("org.ukui.search.service");
    QDBusConnection::sessionBus().registerService("org.ukui.search.service");
    QDBusConnection::sessionBus().registerObject("/lineEdit/textChanged", this, QDBusConnection :: ExportAllSlots | QDBusConnection :: ExportAllSignals);

    connect(this, &QLineEdit::textChanged, this, &SearchLineEdit::lineEditTextChanged);
    connect(this, &QLineEdit::textChanged, this, [ = ]() {
        m_isRecorded = false;
    });
}

void SearchLineEdit::record() {
    if(m_isRecorded == true || text().size() <= 1 || text().isEmpty())
        return;
    GlobalSettings::getInstance()->setSearchRecord(text(), QDateTime::currentDateTime());
    m_isRecorded = true;
    m_model->setStringList(GlobalSettings::getInstance()->getSearchRecord());
}

SearchLineEdit::~SearchLineEdit() {

}

/**
 * @brief lineEditTextChange 监听到搜索框文字的textChanged信号，发送dbus信号给其他程序
 * @param arg 搜索框的文本
 *
 * 需要此点击信号的应用需要做如下绑定
 * QDBusConnection::sessionBus().connect(QString(), QString("/lineEdit/textChanged"), "org.ukui.search.inputbox", "InputBoxTextChanged", this, SLOT(client_get(QString)));
 * 在槽函数client_get(void)　中处理接受到的点击信号
 */
void SearchLineEdit::lineEditTextChanged(QString arg) {
    QDBusMessage message = QDBusMessage::createSignal("/lineEdit/textChanged", "org.ukui.search.inputbox", "InputBoxTextChanged");
    message << arg;
    QDBusConnection::sessionBus().send(message);
}
