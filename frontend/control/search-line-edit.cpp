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
#include "search-line-edit.h"
#include <KWindowEffects>

QT_BEGIN_NAMESPACE
extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);
QT_END_NAMESPACE
using namespace Zeeker;
/**
 * @brief UKuiSearchLineEdit  全局搜索的输入框
 */
SearchLineEdit::SearchLineEdit(QWidget *parent) : QLineEdit(parent) {
    setStyle(new LineEditStyle());
    this->setFocusPolicy(Qt::StrongFocus);
    this->setFixedSize(720, 48);
    this->setTextMargins(35, 0, 0, 0);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setDragEnabled(true);

    m_queryIcon = new QLabel;
    QPixmap pixmap;
    if (!QIcon::fromTheme("system-search-symbolic").isNull()) {
        pixmap =  QPixmap(QIcon::fromTheme("system-search-symbolic").pixmap(QSize(21, 21)));
    } else {
        pixmap =  QPixmap(QIcon(":/res/icons/system-search.symbolic.png").pixmap(QSize(21, 21)));
    }
    m_queryIcon->setProperty("useIconHighlightEffect", 0x10);
    m_queryIcon->setFixedSize(pixmap.size());
    m_queryIcon->setPixmap(pixmap);

    m_ly = new QHBoxLayout(this);
    m_ly->addSpacing(4);
    m_ly->addWidget(m_queryIcon);
    m_ly->addStretch();

    this->setPlaceholderText(tr("Search"));
    this->setMaxLength(100);

    m_timer = new QTimer;
    connect(m_timer, &QTimer::timeout, this, [ = ]() {
        m_timer->stop();
        Q_EMIT this->requestSearchKeyword(this->text());
    });
    connect(this, &SearchLineEdit::textChanged, this, [ = ](QString text) {
        if (m_isEmpty) {
            m_isEmpty = false;
            Q_EMIT this->requestSearchKeyword(text);
        } else {
            if (text == "") {
                m_isEmpty = true;
                m_timer->stop();
                Q_EMIT this->requestSearchKeyword(text);
                return;
            }
            m_timer->start(0.1 * 1000);
        }
    });
}

SearchLineEdit::~SearchLineEdit() {

}

void SearchLineEdit::paintEvent(QPaintEvent *e)
{
//    QPainter p(this);
//    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
//    p.setBrush(palette().base());
//    p.setOpacity(GlobalSettings::getInstance()->getValue(TRANSPARENCY_KEY).toDouble());
//    p.setPen(Qt::NoPen);
//    p.drawRoundedRect(this->rect(), this->height(), this->height());
    return QLineEdit::paintEvent(e);
}

void SearchLineEdit::focusOutEvent(QFocusEvent *e)
{
    this->setFocus();
}

SeachBarWidget::SeachBarWidget(QWidget *parent): QWidget(parent) {
    m_ly = new QHBoxLayout(this);
    m_searchLineEdit = new SearchLineEdit(this);
    this->setFixedSize(m_searchLineEdit->width(), m_searchLineEdit->height());
    m_ly->setContentsMargins(0,0,0,0);
    m_ly->addWidget(m_searchLineEdit);
    this->setFocusProxy(m_searchLineEdit);
    connect(m_searchLineEdit, &SearchLineEdit::requestSearchKeyword, this, &SeachBarWidget::requestSearchKeyword);
}

SeachBarWidget::~SeachBarWidget() {
}

void SeachBarWidget::clear()
{
    m_searchLineEdit->clear();
}

void SeachBarWidget::reSearch()
{
    Q_EMIT this->m_searchLineEdit->requestSearchKeyword(m_searchLineEdit->text());
}

void SeachBarWidget::setText(QString keyword)
{
    m_searchLineEdit->setText(keyword);
}

void SeachBarWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)

//    QPainter p(this);
//    p.setRenderHint(QPainter::Antialiasing);
//    QPainterPath rectPath;
//    rectPath.addRoundedRect(this->rect().adjusted(10, 10, -10, -10), 6, 6);


//    // 画一个黑底
//    QPixmap pixmap(this->rect().size());
//    pixmap.fill(Qt::transparent);
//    QPainter pixmapPainter(&pixmap);
//    pixmapPainter.setRenderHint(QPainter::Antialiasing);
//    //      pixmapPainter.setCompositionMode(QPainter::CompositionMode_Difference);
//    pixmapPainter.setPen(Qt::transparent);
//    pixmapPainter.setBrush(Qt::black);
//    pixmapPainter.setOpacity(0.65);
//    pixmapPainter.drawPath(rectPath);
//    pixmapPainter.end();


//    // 模糊这个黑底
//    QImage img = pixmap.toImage();
//    qt_blurImage(img, 10, false, false);


//    // 挖掉中心
//    pixmap = QPixmap::fromImage(img);
//    QPainter pixmapPainter2(&pixmap);
//    pixmapPainter2.setRenderHint(QPainter::Antialiasing);
//    pixmapPainter2.setCompositionMode(QPainter::CompositionMode_Clear);
//    pixmapPainter2.setPen(Qt::transparent);
//    pixmapPainter2.setBrush(Qt::transparent);
//    pixmapPainter2.drawPath(rectPath);


//    // 绘制阴影
//    p.drawPixmap(this->rect(), pixmap, pixmap.rect());
}

void LineEditStyle::drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    switch (element) {
    case PE_PanelLineEdit://UKUI Text edit style
    {
        if (const QStyleOptionFrame *f = qstyleoption_cast<const QStyleOptionFrame *>(option)) {
            const bool enable = f->state & State_Enabled;
            const bool focus = f->state & State_HasFocus;

            if (!enable) {
                painter->save();
                painter->setPen(Qt::NoPen);
                painter->setBrush(f->palette.brush(QPalette::Disabled, QPalette::Button));
                painter->setRenderHint(QPainter::Antialiasing, true);
                painter->drawRoundedRect(option->rect, 4, 4);
                painter->restore();
                return;
            }

            if (f->state & State_ReadOnly) {
                painter->save();
                painter->setPen(Qt::NoPen);
                painter->setBrush(f->palette.brush(QPalette::Active, QPalette::Button));
                painter->setRenderHint(QPainter::Antialiasing, true);
                painter->drawRoundedRect(option->rect, 24, 24);
                painter->restore();
                return;
            }

            if (focus) {
                painter->save();
                painter->setPen(QPen(f->palette.brush(QPalette::Active, QPalette::Highlight),
                                     2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                painter->setBrush(option->palette.brush(QPalette::Active, QPalette::Base));
                painter->setRenderHint(QPainter::Antialiasing, true);
                painter->drawRoundedRect(option->rect.adjusted(1, 1, -1, -1), 24, 24);
                painter->restore();
            } else {
                QStyleOptionButton button;
                button.state = option->state & ~(State_Sunken | State_On);
                button.rect = option->rect;
                proxy()->drawPrimitive(PE_PanelButtonCommand, &button, painter, widget);
            }
            return;
        }
        break;
    }
    default:
        return;
//        return QProxyStyle::drawPrimitive(element, option, painter, widget);
    }
}
