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
#ifndef SEARCHLINEEDIT_H
#define SEARCHLINEEDIT_H
#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QtDBus/QtDBus>
#include <QPainter>
#include <QAction>
#include <QTimer>
#include <QStringListModel>
#include <QCompleter>
#include <QAbstractItemView>
#include <QVector4D>
#include <QListView>
#include <QProxyStyle>
#include "global-settings.h"

namespace UkuiSearch {

class SearchLineEdit : public QLineEdit {
    Q_OBJECT
public:
    SearchLineEdit(QWidget *parent = nullptr);
//    void record();
    ~SearchLineEdit();
protected:
    void paintEvent(QPaintEvent *);
    void focusOutEvent(QFocusEvent *);

Q_SIGNALS:
    void requestSearchKeyword(QString text);
private:
    QHBoxLayout *m_ly;
    QLabel *m_queryIcon;
    QTimer *m_timer;
    bool m_isEmpty = true;
};

class SeachBarWidget: public QWidget {
    Q_OBJECT
public:
    SeachBarWidget(QWidget *parent = nullptr);
    ~SeachBarWidget();

    void clear();
    void reSearch();

protected:
    void paintEvent(QPaintEvent *e);

Q_SIGNALS:
    void requestSearchKeyword(QString text);

public Q_SLOTS:
    void setText(QString keyword);

private:
    SearchLineEdit *m_searchLineEdit;
    QHBoxLayout *m_ly;
};



class LineEditStyle : public QProxyStyle
{
public:
    LineEditStyle() {}
    void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
};
}

#endif //SEARCHLINEEDIT_H
