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
#ifndef RESULTVIEWDELEGATE_H
#define RESULTVIEWDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QStyle>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QProxyStyle>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegExp>
#include "global-settings.h"

namespace UkuiSearch {
class HightLightEffectHelper : public QSyntaxHighlighter
{
public:
    explicit HightLightEffectHelper(QObject *parent = nullptr);
    void setExpression(const QString &text);
    void setTextColor(const QBrush &brush);

protected:
    void highlightBlock(const QString &text);

private:
    QRegExp m_expression;
    QTextCharFormat m_textCharFormat;
};

class ResultViewDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ResultViewDelegate(QObject *parent = nullptr);
    ~ResultViewDelegate() = default;
    void setSearchKeyword(const QString &);
protected:
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paint(QPainter *, const QStyleOptionViewItem &, const QModelIndex &) const override;

private:
    QTextDocument *m_textDoc = nullptr;
    HightLightEffectHelper *m_hightLightEffectHelper = nullptr;

};

class ResultItemStyle : public QProxyStyle
{
public:
    static ResultItemStyle *getStyle();
    void drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const;
    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
private:
    explicit ResultItemStyle() {}
    ~ResultItemStyle() = default;
};
}

#endif // RESULTVIEWDELEGATE_H
