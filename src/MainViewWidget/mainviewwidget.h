/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */

#ifndef MAINVIEWWIDGET_H
#define MAINVIEWWIDGET_H

#include <QWidget>
#include <QSpacerItem>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QDesktopWidget>
#include <QKeyEvent>
#include <QScrollBar>
#include <QFileSystemWatcher>
#include <QSettings>
#include "src/Interface/ukuimenuinterface.h"
#include "src/AppSearch/searchresultwidget.h"
#include "src/AppSearch/searchappthread.h"
#include "src/Style/style.h"
#include "../FileSearch/filemodel.h"
#include "../SearchSetting/settingmodel.h"
#include <QTreeView>

class MainViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainViewWidget(QWidget *parent = nullptr);
    ~MainViewWidget();
    /**
     * @brief Load the default main view
     */
    void loadMinMainView();
    /**
     * @brief Initializes the interface state
     */
    void widgetMakeZero();

    void initSearchWidget();
    void AddSearchWidget();

private:
    QVBoxLayout* mainLayout;
    QWidget *m_topWidget=nullptr;
    QHBoxLayout *m_topLayout=nullptr;

    QSpacerItem *m_verticalSpacer=nullptr;

    QLineEdit *m_queryLineEdit=nullptr;
    QWidget *m_queryWid=nullptr;
    QLabel *m_queryIcon=nullptr;
    QLabel *m_queryText=nullptr;
    bool m_isSearching;
    QString m_searchKeyWords;
    QPropertyAnimation *m_animation=nullptr;

    SearchResultWidget *m_searchResultWid=nullptr;
    SearchAppThread *m_searchAppThread=nullptr;

    int m_widgetState=1;//Classification window number
    int m_saveCurrentWidState=-1;//Store the current category window number

    bool m_isFullScreen=false;
    bool m_isHiden=false;

    QSettings *m_setting=nullptr;
    QGSettings *m_gsetting=nullptr;

    QTreeView *m_fileview; //文件view
    QTreeView *m_settingview;//设置view

    filemodel *m_filemodel;//文件model

    settingModel *m_settingmodel;//设置model

    int fileNum; //记录搜索出来的文件数量
    int SettingNum;//记录搜索出来的设置数量

   QPushButton *search_web_page;

   QString search1;





protected:
    /**
     * @brief Initializes UI
     */
    void initUi();
    /**
     * @brief Add a top window control
     */
    void addTopControl();
    /**
     * @brief Initializes the query box
     */
    void initQueryLineEdit();

    void changesize(); //实现改变文件与设置view大小的方法

public Q_SLOTS:
    /**
     * @brief Search box focus event filtering
     */
    bool eventFilter(QObject *watched, QEvent *event);
    /**
     * @brief Load the full screen letter classification interface
     */
    /**
     * @brief Respond to search box
     * @param arg: Search keywords
     */
    void searchAppSlot(QString arg);
    /**
     * @brief Respond to search box animation finish
     */
    void animationFinishedSlot();
    /**
     * @brief Receive search results
     * @param arg: Search results
     */
    void recvSearchResult(QVector<QStringList> arg);

    void setFileView(int row); //改变文件view大小的槽函数

    void setSettingView(int row);//改变设置view大小的槽函数



Q_SIGNALS:
    /**
     * @brief Send the search keyword to the SearchAppThread
     * @param arg: Search keyword
     */
    void sendSearchKeyword(QString arg);
};

#endif // MAINVIEWWIDGET_H
