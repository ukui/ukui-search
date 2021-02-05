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
#ifndef CONTENTWIDGET_H
#define CONTENTWIDGET_H

#include <QObject>
#include <QStackedWidget>
#include <QScrollArea>
#include <QGridLayout>
#include "control/search-detail-view.h"
#include "home-page-item.h"
#include "show-more-label.h"
#include "title-label.h"

class ContentWidget : public QStackedWidget
{
    Q_OBJECT
public:
    ContentWidget(QWidget *);
    ~ContentWidget();

    void setPage(const int&);
    int currentPage();
    void refreshSearchList(const QVector<QStringList>&);
    void appendSearchItem(const int& type, const QString& path, QStringList contents = QStringList());
    void initHomePage();
    void setContentList(const QStringList&);
    void setKeyword(QString);
    void setQuicklyOpenList(const QStringList &);
private:
    void initUI();
    void initListView();
    void hideListView();
    void setupConnect(SearchListView *);
    void clearHomepage();
    void resetListHeight();
    QString m_keyword;
    QStringList m_contentDetailList;
    QWidget * m_homePage = nullptr;
    QVBoxLayout * m_homePageLyt = nullptr;
    QWidget * m_resultPage = nullptr;
    QHBoxLayout * m_resultPageLyt = nullptr;
    QScrollArea * m_resultListArea = nullptr;
    QScrollArea * m_resultDetailArea = nullptr;
    QWidget * m_resultList = nullptr;
    QVBoxLayout * m_listLyt = nullptr;
    QWidget * m_resultDetail = nullptr;
    QVBoxLayout * m_detailLyt = nullptr;

    SearchDetailView * m_detailView = nullptr;
    SearchListView * m_fileListView = nullptr;
    SearchListView * m_dirListView = nullptr;
    SearchListView * m_contentListView = nullptr;
    SearchListView * m_settingListView = nullptr;
    SearchListView * m_appListView = nullptr;
    SearchListView * m_bestListView = nullptr;
    TitleLabel * m_fileTitleLabel = nullptr;
    TitleLabel * m_dirTitleLabel = nullptr;
    TitleLabel * m_contentTitleLabel = nullptr;
    TitleLabel * m_appTitleLabel = nullptr;
    TitleLabel * m_settingTitleLabel = nullptr;
    TitleLabel * m_bestTitleLabel = nullptr;
    ShowMoreLabel * m_appShowMoreLabel = nullptr;
    ShowMoreLabel * m_settingShowMoreLabel = nullptr;
    ShowMoreLabel * m_dirShowMoreLabel = nullptr;
    ShowMoreLabel * m_fileShowMoreLabel = nullptr;
    ShowMoreLabel * m_contentShowMoreLabel = nullptr;

    QStringList m_appList;
    QStringList m_settingList;
    QStringList m_dirList;
    QStringList m_fileList;
    QStringList m_contentList;
    QStringList m_quicklyOpenList;

    QString m_bestContent; //最佳匹配有文件内容搜索结果的时候，以此变量传递

    int m_currentType = 0;

    QString getTitleName(const int&);

Q_SIGNALS:
    void currentItemChanged();

private Q_SLOTS:
    void clearLayout(QLayout *);
};

#endif // CONTENTWIDGET_H
