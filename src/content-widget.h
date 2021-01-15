#ifndef CONTENTWIDGET_H
#define CONTENTWIDGET_H

#include <QObject>
#include <QStackedWidget>
#include <QScrollArea>
#include <QGridLayout>
#include "control/search-detail-view.h"
#include "home-page-item.h"

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
private:
    void initUI();
    void initListView();
    void hideListView();
    void setupConnect(SearchListView *);
    void clearHomepage();
    void resetListHeight();
    QString m_keyword;
    QStringList m_contentList;
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
    QLabel * m_fileTitleLabel = nullptr;
    QLabel * m_dirTitleLabel = nullptr;
    QLabel * m_contentTitleLabel = nullptr;
    QLabel * m_appTitleLabel = nullptr;
    QLabel * m_settingTitleLabel = nullptr;
    QLabel * m_bestTitleLabel = nullptr;

    int m_currentType = 0;

    QString getTitleName(const int&);

Q_SIGNALS:
    void currentItemChanged();

private Q_SLOTS:
    void clearLayout(QLayout *);
};

#endif // CONTENTWIDGET_H
