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
#include "global-settings.h"

class SearchLineEdit;

class SeachBarWidget:public QWidget
{
public:
  SeachBarWidget(QWidget *parent = nullptr);
  ~SeachBarWidget();
};

class SeachBar:public QWidget
{
  public:
    SeachBar();
    ~SeachBar();

private:
//    QLineEdit *m_queryLineEdit=nullptr;
};

class SearchBarHLayout : public QHBoxLayout
{
    Q_OBJECT
public:
    SearchBarHLayout(QWidget *parent = nullptr);
    ~SearchBarHLayout();
    void clearText();
    QString text();
    void focusIn();
    void focusOut();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    void initUI();
    bool m_isEmpty = true;
    QTimer * m_timer = nullptr;

    SearchLineEdit * m_queryLineEdit = nullptr;
    QPropertyAnimation * m_animation = nullptr;
    QWidget * m_queryWidget = nullptr;
    QLabel * m_queryIcon = nullptr;
    QLabel * m_queryText = nullptr;
    bool m_isSearching = false;

Q_SIGNALS:
    void textChanged(QString text);
public Q_SLOTS:
    void effectiveSearchRecord();

};
class SearchBarWidgetLayout : public QHBoxLayout
{
public:
    SearchBarWidgetLayout();
    ~SearchBarWidgetLayout();
private:
    void initUI();

};

class SearchLineEdit : public QLineEdit
{
    Q_OBJECT

    /*
     * 负责与ukui桌面环境应用通信的dbus
     * 搜索框文本改变的时候发送信号
　　　*/
    Q_CLASSINFO("D-Bus Interface", "org.ukui.search.inputbox")
public:
    SearchLineEdit();
    void record();
    ~SearchLineEdit();

private Q_SLOTS:
    void  lineEditTextChanged(QString arg);
private:
    QStringListModel *m_model = nullptr;
    QCompleter *m_completer= nullptr;
    bool m_isRecorded = false;
};
