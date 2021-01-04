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
    void refreshSearchList(const QVector<int>&, const QVector<QStringList>&);
    void initHomePage(const QVector<QStringList>&);
    void setContentList(const QStringList&);
private:
    void initUI();
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

    int m_currentType = 0;

    QString getTitleName(const int&);

Q_SIGNALS:
    void currentItemChanged();

private Q_SLOTS:
    void clearSearchList();
};

#endif // CONTENTWIDGET_H
