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
#include "global-settings.h"

namespace Zeeker {
class ResultViewDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit ResultViewDelegate(QObject *parent = nullptr);
    ~ResultViewDelegate() = default;
    void setSearchKeyword(const QString &);
protected:
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
    QString m_regFindKeyWords = 0;
    void paint(QPainter *, const QStyleOptionViewItem &, const QModelIndex &) const override;
    QString getHtmlText(QPainter *, const QStyleOptionViewItem &, const QModelIndex &) const;
    QString escapeHtml(const QString&) const;
};
}

#endif // RESULTVIEWDELEGATE_H
